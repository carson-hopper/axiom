#pragma once

#include "Axiom/Environment/Level/Generator/ChunkGenerator.h"
#include "Axiom/Environment/Level/Generator/AnvilReader.h"
#include "Axiom/Environment/Level/Generator/BlockRegistry.h"
#include "Axiom/Environment/Level/Generator/BlockStates.h"
#include "Axiom/Environment/Level/ChunkEncoder.h"
#include "Axiom/Core/PathUtil.h"
#include "Axiom/Core/Log.h"
#include "Axiom/Data/Nbt/NbtCompound.h"
#include "Axiom/Data/Nbt/NbtIo.h"
#include "Axiom/Data/Nbt/NbtList.h"
#include "Axiom/Data/Nbt/NbtListImpl.h"
#include "Axiom/Environment/Level/Physics/WorldTicker.h"

#include <array>
#include <mutex>
#include <string>
#include <unordered_map>

namespace Axiom {

	/**
	 * Loads pre-generated chunks from a vanilla Minecraft world.
	 * Reads .mca region files and extracts block/biome data directly,
	 * re-encoding them into the network protocol format.
	 */
	class VanillaChunkGenerator : public ChunkGenerator {
	public:
		explicit VanillaChunkGenerator(const std::string& worldDirectory)
			: m_AnvilReader(worldDirectory + "/region") {

			const auto dataPath = ResolvePath("data");
			m_BlockRegistry.LoadFromExtractorData(dataPath.string());
		}

		ChunkData Generate(const int32_t chunkX, const int32_t chunkZ) override {
			const auto nbtData = m_AnvilReader.ReadChunkNbt(chunkX, chunkZ);

			if (!nbtData) {
				return GenerateEmptyChunk(chunkX, chunkZ);
			}

			return ParseChunkNbt(chunkX, chunkZ, *nbtData);
		}

		double SpawnY() const override {
			return 100.0;
		}

		/**
		 * Save the chunk with player modifications back to the region file.
		 * Rebuilds the entire NBT tree from the in-memory block state via
		 * the new NbtCompound / NbtList API.
		 */
		void SaveChunk(int32_t chunkX, int32_t chunkZ, const class WorldTicker& ticker) const {
			auto root = CreateRef<NbtCompound>();
			root->PutInt("DataVersion", 4325);
			root->PutString("Status", "minecraft:full");

			auto sections = CreateRef<NbtList>(NbtTagType::Compound);
			for (int sectionY = -4; sectionY < 20; sectionY++) {
				auto section = BuildSectionNbt(chunkX, chunkZ, sectionY, ticker);
				sections->Add(section);
			}
			root->PutList("sections", sections);

			NetworkBuffer output;
			NbtIo::WriteFile(root, output);
			m_AnvilReader.WriteChunkNbt(chunkX, chunkZ, output.Data());

			{
				std::lock_guard<std::mutex> lock(m_BlockCacheMutex);
				m_BlockCache.erase(ChunkCacheKey(chunkX, chunkZ));
			}

			AX_CORE_INFO("Saved modified chunk ({}, {})", chunkX, chunkZ);
		}

		int32_t GetBlockAt(int32_t worldX, int32_t worldY, int32_t worldZ) const override {
			const int32_t chunkX = worldX >> 4;
			const int32_t chunkZ = worldZ >> 4;
			const int64_t key = ChunkCacheKey(chunkX, chunkZ);

			std::lock_guard<std::mutex> lock(m_BlockCacheMutex);
			const auto iterator = m_BlockCache.find(key);
			if (iterator == m_BlockCache.end()) {
				return BlockState::Air;
			}

			const int localX = worldX & 0xF;
			const int localZ = worldZ & 0xF;
			const int sectionIndex = (worldY + 64) / 16;
			if (sectionIndex < 0 || sectionIndex >= 24) {
				return BlockState::Air;
			}

			const int localY = (worldY + 64) & 0xF;
			const int blockIndex = (localY * 16 + localZ) * 16 + localX;
			return iterator->second[sectionIndex * 4096 + blockIndex];
		}

	private:
		ChunkData GenerateEmptyChunk(const int32_t chunkX, const int32_t chunkZ) const {
			NetworkBuffer sectionData;
			for (int sectionIndex = 0; sectionIndex < 24; sectionIndex++) {
				ChunkEncoder::EncodeSingleBlockSection(sectionData, BlockState::Air, 0);
			}
			return ChunkData{chunkX, chunkZ, std::move(sectionData.Data()), 0, 0};
		}

		/**
		 * Build a single section compound from the current world state.
		 * Reads every block via WorldTicker::GetBlock() so player edits
		 * are captured.
		 */
		Ref<NbtCompound> BuildSectionNbt(int32_t chunkX, int32_t chunkZ, int sectionY,
			const WorldTicker& ticker) const {

			std::vector<std::string> paletteNames;
			std::unordered_map<std::string, int32_t> paletteMap;
			std::array<int32_t, 4096> paletteIndices{};

			const int baseWorldY = (sectionY + 4) * 16 - 64;
			const int baseWorldX = chunkX * 16;
			const int baseWorldZ = chunkZ * 16;

			for (int localY = 0; localY < 16; localY++) {
				for (int localZ = 0; localZ < 16; localZ++) {
					for (int localX = 0; localX < 16; localX++) {
						const int worldX = baseWorldX + localX;
						const int worldY = baseWorldY + localY;
						const int worldZ = baseWorldZ + localZ;

						const int32_t stateId = ticker.GetBlock(worldX, worldY, worldZ);
						std::string blockName = m_BlockRegistry.GetBlockName(stateId);
						if (blockName.empty()) {
							blockName = "minecraft:air";
						}

						const auto iterator = paletteMap.find(blockName);
						int32_t paletteIndex = 0;
						if (iterator == paletteMap.end()) {
							paletteIndex = static_cast<int32_t>(paletteNames.size());
							paletteNames.push_back(blockName);
							paletteMap[blockName] = paletteIndex;
						} else {
							paletteIndex = iterator->second;
						}

						const int blockIndex = (localY * 16 + localZ) * 16 + localX;
						paletteIndices[blockIndex] = paletteIndex;
					}
				}
			}

			auto section = CreateRef<NbtCompound>();
			section->PutByte("Y", static_cast<int8_t>(sectionY));

			// block_states
			auto blockStates = CreateRef<NbtCompound>();

			auto palette = CreateRef<NbtList>(NbtTagType::Compound);
			for (const auto& name : paletteNames) {
				auto entry = CreateRef<NbtCompound>();
				entry->PutString("Name", name);
				palette->Add(entry);
			}
			blockStates->PutList("palette", palette);

			if (paletteNames.size() > 1) {
				int bitsPerEntry = 4;
				while ((1 << bitsPerEntry) < static_cast<int>(paletteNames.size())) {
					bitsPerEntry++;
				}

				const int entriesPerLong = 64 / bitsPerEntry;
				const int longCount = (4096 + entriesPerLong - 1) / entriesPerLong;

				std::vector<int64_t> packed(longCount, 0);
				for (int i = 0; i < 4096; i++) {
					const int longIndex = i / entriesPerLong;
					const int bitOffset = (i % entriesPerLong) * bitsPerEntry;
					packed[longIndex] |= static_cast<int64_t>(paletteIndices[i]) << bitOffset;
				}

				blockStates->PutLongArray("data", std::move(packed));
			}
			section->PutCompound("block_states", blockStates);

			// biomes — single plains for now
			auto biomes = CreateRef<NbtCompound>();
			auto biomePalette = CreateRef<NbtList>(NbtTagType::String);
			biomePalette->Add(CreateRef<NbtString>("minecraft:plains"));
			biomes->PutList("palette", biomePalette);
			section->PutCompound("biomes", biomes);

			return section;
		}

		/**
		 * Parse the raw NBT bytes from a region file into a network-ready
		 * ChunkData. Uses the new NbtCompound tree for clean tag access.
		 */
		ChunkData ParseChunkNbt(const int32_t chunkX, const int32_t chunkZ,
			const std::vector<uint8_t>& nbtData) const {

			const int64_t cacheKey = ChunkCacheKey(chunkX, chunkZ);
			{
				std::lock_guard<std::mutex> lock(m_BlockCacheMutex);
				m_BlockCache[cacheKey].resize(24 * 4096, 0);
			}

			NetworkBuffer input(nbtData);
			auto root = NbtIo::ReadFile(input);
			if (!root) {
				return GenerateEmptyChunk(chunkX, chunkZ);
			}

			auto sectionsList = root->GetList("sections");
			if (!sectionsList) {
				return GenerateEmptyChunk(chunkX, chunkZ);
			}

			// Index the sections by Y for sparse access.
			std::unordered_map<int8_t, Ref<NbtCompound>> sectionByY;
			for (size_t index = 0; index < sectionsList->Size(); index++) {
				auto section = sectionsList->GetCompound(index);
				if (!section) continue;
				sectionByY[section->GetByte("Y")] = section;
			}

			NetworkBuffer sectionData;
			for (int targetY = -4; targetY < 20; targetY++) {
				const int sectionIndex = targetY + 4;
				const auto iterator = sectionByY.find(static_cast<int8_t>(targetY));
				if (iterator != sectionByY.end()) {
					EncodeSectionFromNbt(iterator->second, sectionData, cacheKey, sectionIndex);
				} else {
					ChunkEncoder::EncodeSingleBlockSection(sectionData, BlockState::Air, 0);
				}
			}

			return ChunkData{chunkX, chunkZ, std::move(sectionData.Data()), 128, 0};
		}

		void EncodeSectionFromNbt(const Ref<NbtCompound>& section, NetworkBuffer& output,
			int64_t cacheKey, int sectionIndex) const {

			auto blockStates = section->GetCompound("block_states");
			if (blockStates) {
				EncodeBlockStatesFromNbt(blockStates, output, cacheKey, sectionIndex);
			} else {
				output.WriteShort(0);
				output.WriteShort(0);
				output.WriteByte(0);
				output.WriteVarInt(0);
			}

			auto biomes = section->GetCompound("biomes");
			if (biomes) {
				EncodeBiomesFromNbt(biomes, output);
			} else {
				output.WriteByte(0);
				output.WriteVarInt(0);
			}
		}

		void EncodeBlockStatesFromNbt(const Ref<NbtCompound>& blockStates, NetworkBuffer& output,
			int64_t cacheKey, int sectionIndex) const {

			std::vector<std::string> paletteNames;
			auto palette = blockStates->GetList("palette");
			if (palette) {
				for (size_t index = 0; index < palette->Size(); index++) {
					auto entry = palette->GetCompound(index);
					if (entry) {
						paletteNames.push_back(entry->GetString("Name"));
					}
				}
			}

			std::vector<int64_t> dataArray = blockStates->GetLongArray("data");

			{
				std::lock_guard<std::mutex> lock(m_BlockCacheMutex);
				CacheDecodedSection(cacheKey, sectionIndex, paletteNames, dataArray,
					dataArray.empty() ? 0 : static_cast<int>(dataArray.size() * 64 / 4096));
			}

			int16_t nonAirCount = 0;
			for (const auto& name : paletteNames) {
				if (name != "minecraft:air" && name != "minecraft:cave_air" && name != "minecraft:void_air") {
					nonAirCount = 4096;
					break;
				}
			}

			if (paletteNames.size() <= 1) {
				const bool isAir = paletteNames.empty() || paletteNames[0] == "minecraft:air";
				output.WriteShort(isAir ? 0 : 4096);
				output.WriteShort(0);
				output.WriteByte(0);
				output.WriteVarInt(isAir ? 0 : BlockNameToStateId(paletteNames[0]));
				return;
			}

			output.WriteShort(nonAirCount);
			output.WriteShort(0);

			if (dataArray.empty()) {
				output.WriteByte(0);
				output.WriteVarInt(0);
				return;
			}

			int bitsPerEntry = static_cast<int>(dataArray.size() * 64 / 4096);
			if (bitsPerEntry < 4) bitsPerEntry = 4;
			if (bitsPerEntry > 15) bitsPerEntry = 15;

			output.WriteByte(static_cast<uint8_t>(bitsPerEntry));
			output.WriteVarInt(static_cast<int32_t>(paletteNames.size()));
			for (const auto& name : paletteNames) {
				output.WriteVarInt(BlockNameToStateId(name));
			}

			for (const int64_t value : dataArray) {
				output.WriteLong(value);
			}
		}

		void EncodeBiomesFromNbt(const Ref<NbtCompound>& biomes, NetworkBuffer& output) const {
			std::vector<std::string> paletteNames;
			auto palette = biomes->GetList("palette");
			if (palette) {
				for (size_t index = 0; index < palette->Size(); index++) {
					paletteNames.push_back(palette->GetString(index));
				}
			}

			std::vector<int64_t> dataArray = biomes->GetLongArray("data");

			if (paletteNames.size() <= 1) {
				output.WriteByte(0);
				output.WriteVarInt(paletteNames.empty() ? 0 : BiomeNameToId(paletteNames[0]));
				return;
			}

			int bitsPerEntry = 1;
			while ((1 << bitsPerEntry) < static_cast<int>(paletteNames.size())) {
				bitsPerEntry++;
			}
			if (bitsPerEntry > 3) {
				bitsPerEntry = 6;
			}

			output.WriteByte(static_cast<uint8_t>(bitsPerEntry));

			if (bitsPerEntry <= 3) {
				output.WriteVarInt(static_cast<int32_t>(paletteNames.size()));
				for (const auto& name : paletteNames) {
					output.WriteVarInt(BiomeNameToId(name));
				}
			}

			for (const int64_t value : dataArray) {
				output.WriteLong(value);
			}
		}

		int32_t BlockNameToStateId(const std::string& name) const {
			return m_BlockRegistry.GetBlockStateId(name);
		}

		int32_t BiomeNameToId(const std::string& name) const {
			return m_BlockRegistry.GetBiomeId(name);
		}

		void CacheDecodedSection(int64_t chunkKey, int sectionIndex,
			const std::vector<std::string>& paletteNames,
			const std::vector<int64_t>& dataArray, int bitsPerEntry) const {

			auto& blocks = m_BlockCache[chunkKey];
			if (blocks.empty()) {
				blocks.resize(24 * 4096, 0);
			}

			const int baseOffset = sectionIndex * 4096;

			if (paletteNames.size() <= 1) {
				const int32_t stateId = paletteNames.empty() ? 0 : BlockNameToStateId(paletteNames[0]);
				for (int i = 0; i < 4096; i++) {
					blocks[baseOffset + i] = stateId;
				}
				return;
			}

			if (dataArray.empty() || bitsPerEntry <= 0) return;

			const int entriesPerLong = 64 / bitsPerEntry;
			const int64_t mask = (1LL << bitsPerEntry) - 1;

			for (int i = 0; i < 4096; i++) {
				const int longIndex = i / entriesPerLong;
				const int bitOffset = (i % entriesPerLong) * bitsPerEntry;

				if (longIndex >= static_cast<int>(dataArray.size())) break;

				const int paletteIndex = static_cast<int>((dataArray[longIndex] >> bitOffset) & mask);
				if (paletteIndex < static_cast<int>(paletteNames.size())) {
					blocks[baseOffset + i] = BlockNameToStateId(paletteNames[paletteIndex]);
				}
			}
		}

		static int64_t ChunkCacheKey(int32_t chunkX, int32_t chunkZ) {
			return (static_cast<int64_t>(chunkX) << 32) | static_cast<uint32_t>(chunkZ);
		}

		mutable std::mutex m_BlockCacheMutex;
		mutable std::unordered_map<int64_t, std::vector<int32_t>> m_BlockCache;
		mutable AnvilReader m_AnvilReader;
		BlockRegistry m_BlockRegistry;
	};

}
