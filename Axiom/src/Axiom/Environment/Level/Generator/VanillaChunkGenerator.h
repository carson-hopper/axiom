#pragma once

#include "Axiom/Environment/Level/Generator/ChunkGenerator.h"
#include "Axiom/Environment/Level/Generator/AnvilReader.h"
#include "Axiom/Environment/Level/Generator/BlockRegistry.h"
#include "Axiom/Environment/Level/Generator/BlockStates.h"
#include "Axiom/Environment/Level/ChunkEncoder.h"
#include "Axiom/Core/PathUtil.h"
#include "Axiom/Core/Log.h"
#include "Axiom/Network/Nbt/NbtReader.h"
#include "Axiom/Network/Nbt/NbtWriter.h"
#include "Axiom/Environment/Level/Physics/WorldTicker.h"

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
			AX_CORE_INFO("VanillaChunkGenerator: loaded {} blocks, {} biomes",
				m_BlockRegistry.BlockCount(), m_BlockRegistry.BiomeCount());
		}

		ChunkData Generate(const int32_t chunkX, const int32_t chunkZ) override {
			auto nbtData = m_AnvilReader.ReadChunkNbt(chunkX, chunkZ);

			if (!nbtData) {
				return GenerateEmptyChunk(chunkX, chunkZ);
			}

			return ParseChunkNbt(chunkX, chunkZ, *nbtData);
		}

		double SpawnY() const override {
			return 100.0;  // Will be overridden by level.dat later
		}

		/**
		 * Save the chunk back to the region file.
		 * Re-reads the original NBT and writes it back (preserving it).
		 * A full implementation would rebuild the NBT from modified block data.
		 */
		/**
		 * Save the chunk with player modifications back to the region file.
		 * Rebuilds section NBT from the current block state (cache + overrides).
		 */
		void SaveChunk(int32_t chunkX, int32_t chunkZ, class WorldTicker& ticker) {
			NbtWriter writer;
			writer.BeginRootCompound();

			// DataVersion
			writer.WriteInt("DataVersion", 4325); // 26.1

			// Status
			writer.WriteString("Status", "minecraft:full");

			// Sections list: 24 sections from Y=-4 to Y=19
			writer.BeginList("sections", 10, 24);
			for (int sectionY = -4; sectionY < 20; sectionY++) {
				// Each section is a compound in the list
				writer.WriteByte("Y", static_cast<int8_t>(sectionY));

				// Build palette + packed data from current block state
				std::vector<std::string> palette;
				std::unordered_map<std::string, int32_t> paletteMap;
				std::array<int32_t, 4096> paletteIndices{};

				int baseWorldY = (sectionY + 4) * 16 - 64;
				int baseWorldX = chunkX * 16;
				int baseWorldZ = chunkZ * 16;

				for (int localY = 0; localY < 16; localY++) {
					for (int localZ = 0; localZ < 16; localZ++) {
						for (int localX = 0; localX < 16; localX++) {
							int worldX = baseWorldX + localX;
							int worldY = baseWorldY + localY;
							int worldZ = baseWorldZ + localZ;

							int32_t stateId = ticker.GetBlock(worldX, worldY, worldZ);
							std::string blockName = m_BlockRegistry.GetBlockName(stateId);
							if (blockName.empty()) blockName = "minecraft:air";

							auto it = paletteMap.find(blockName);
							int32_t paletteIndex;
							if (it == paletteMap.end()) {
								paletteIndex = static_cast<int32_t>(palette.size());
								palette.push_back(blockName);
								paletteMap[blockName] = paletteIndex;
							} else {
								paletteIndex = it->second;
							}

							int blockIndex = (localY * 16 + localZ) * 16 + localX;
							paletteIndices[blockIndex] = paletteIndex;
						}
					}
				}

				// Write block_states compound
				writer.BeginCompound("block_states");

				// Palette
				writer.BeginList("palette", 10, static_cast<int32_t>(palette.size()));
				for (const auto& name : palette) {
					writer.WriteString("Name", name);
					writer.EndCompound(); // end palette entry
				}

				// Data (packed long array) — only if palette > 1
				if (palette.size() > 1) {
					int bitsPerEntry = 4;
					while ((1 << bitsPerEntry) < static_cast<int>(palette.size()))
						bitsPerEntry++;

					int entriesPerLong = 64 / bitsPerEntry;
					int longCount = (4096 + entriesPerLong - 1) / entriesPerLong;

					std::vector<int64_t> packed(longCount, 0);
					for (int i = 0; i < 4096; i++) {
						int longIndex = i / entriesPerLong;
						int bitOffset = (i % entriesPerLong) * bitsPerEntry;
						packed[longIndex] |= (static_cast<int64_t>(paletteIndices[i]) << bitOffset);
					}

					writer.WriteLongArray("data", packed);
				}

				writer.EndCompound(); // end block_states

				// Biomes — single-valued plains
				writer.BeginCompound("biomes");
				writer.BeginList("palette", 8, 1);
				writer.WriteStringValue("minecraft:plains");
				writer.EndCompound(); // end biomes

				writer.EndCompound(); // end section
			}

			writer.EndCompound(); // end root

			m_AnvilReader.WriteChunkNbt(chunkX, chunkZ, writer.Data());

			// Clear the block cache so the next load reads from the updated .mca
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
			if (iterator == m_BlockCache.end()) return BlockState::Air;

			const int localX = worldX & 0xF;
			const int localZ = worldZ & 0xF;
			const int sectionIndex = (worldY + 64) / 16;
			if (sectionIndex < 0 || sectionIndex >= 24) return BlockState::Air;

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

		ChunkData ParseChunkNbt(const int32_t chunkX, const int32_t chunkZ,
			const std::vector<uint8_t>& nbtData) const {

			const int64_t cacheKey = ChunkCacheKey(chunkX, chunkZ);
			{
				std::lock_guard<std::mutex> lock(m_BlockCacheMutex);
				m_BlockCache[cacheKey].resize(24 * 4096, 0);
			}

			NbtReader reader(nbtData);

			if (!reader.ReadRootCompound()) {
				return GenerateEmptyChunk(chunkX, chunkZ);
			}

			// Find "sections" list
			NetworkBuffer sectionData;
			int32_t heightmapValue = 128;
			int32_t biomeId = 0;
			bool hasSections = false;

			// Structure: root compound → sections (list of compounds)
			// Each section has: Y (byte), block_states (compound), biomes (compound)
			auto sectionsType = reader.FindTag("sections");
			if (sectionsType == NbtReader::TagList) {
				hasSections = true;
				auto listHeader = reader.ReadListHeader();

				// We need sections indexed by Y. MC stores them in the list
				// ordered by Y value. We need sections for Y -4 to 19 (indices 0-23).
				struct SectionEntry {
					int8_t yIndex;
					size_t dataPosition;
				};
				std::vector<SectionEntry> entries;

				for (int32_t sectionIndex = 0; sectionIndex < listHeader.count; sectionIndex++) {
					const size_t sectionStart = reader.Position();

					// Read Y value
					int8_t yValue = 0;
					auto yType = reader.FindTag("Y");
					if (yType == NbtReader::TagByte) {
						yValue = reader.ReadTagByte();
					}

					entries.push_back({yValue, sectionStart});

					// Skip to end of this compound
					// Reset to start and skip the whole thing
					reader.SetPosition(sectionStart);
					reader.SkipCompound();
				}

				// Now encode 24 sections in order (Y = -4 to 19)
				for (int targetY = -4; targetY < 20; targetY++) {
					const int sectionIndex = targetY + 4;
					bool found = false;
					for (const auto& entry : entries) {
						if (entry.yIndex == targetY) {
							reader.SetPosition(entry.dataPosition);
							EncodeSectionFromNbt(reader, sectionData, cacheKey, sectionIndex);
							found = true;
							break;
						}
					}
					if (!found) {
						// Missing section = all air
						ChunkEncoder::EncodeSingleBlockSection(sectionData, BlockState::Air, 0);
					}
				}
			}

			if (!hasSections) {
				return GenerateEmptyChunk(chunkX, chunkZ);
			}

			return ChunkData{chunkX, chunkZ, std::move(sectionData.Data()), heightmapValue, biomeId};
		}

		void EncodeSectionFromNbt(NbtReader& reader, NetworkBuffer& output,
			int64_t cacheKey, int sectionIndex) const {
			// Parse section compound to find block_states and biomes
			// Then re-encode for the network protocol

			int16_t nonAirCount = 0;
			bool hasBlockStates = false;
			bool hasBiomes = false;

			// We need: block_states.palette (list), block_states.data (long array)
			//          biomes.palette (list), biomes.data (long array)

			size_t sectionStart = reader.Position();

			// First pass: find block_states
			auto blockStatesType = reader.FindTag("block_states");
			if (blockStatesType == NbtReader::TagCompound) {
				hasBlockStates = true;
				EncodeBlockStatesFromNbt(reader, output, nonAirCount, cacheKey, sectionIndex);
			}

			// Find biomes
			reader.SetPosition(sectionStart);
			auto biomesType = reader.FindTag("biomes");
			if (biomesType == NbtReader::TagCompound) {
				hasBiomes = true;
				EncodeBiomesFromNbt(reader, output);
			}

			if (!hasBlockStates) {
				// Write empty block states (single-valued air)
				output.WriteShort(0);
				output.WriteShort(0);
				output.WriteByte(0);
				output.WriteVarInt(0);  // Air
			}

			if (!hasBiomes) {
				output.WriteByte(0);
				output.WriteVarInt(0);  // Plains
			}

			// Skip rest of section compound
			reader.SetPosition(sectionStart);
			reader.SkipCompound();
		}

		void EncodeBlockStatesFromNbt(NbtReader& reader, NetworkBuffer& output,
			int16_t& nonAirCount, int64_t cacheKey, int sectionIndex) const {

			// block_states compound has: palette (list of compounds), data (long array)
			size_t blockStatesStart = reader.Position();

			// Find palette
			auto paletteType = reader.FindTag("palette");
			std::vector<std::string> paletteNames;

			if (paletteType == NbtReader::TagList) {
				auto listHeader = reader.ReadListHeader();
				for (int32_t i = 0; i < listHeader.count; i++) {
					// Each palette entry is a compound with "Name" string and optional "Properties"
					auto nameType = reader.FindTag("Name");
					std::string blockName;
					if (nameType == NbtReader::TagString) {
						blockName = reader.ReadTagString();
					}
					paletteNames.push_back(blockName);

					// Skip rest of the palette entry compound (Properties etc)
					// Actually we're in the middle of the compound, need to finish it
					reader.SkipCompound();
				}
			}

			// Find data long array
			reader.SetPosition(blockStatesStart);
			auto dataType = reader.FindTag("data");
			std::vector<int64_t> dataArray;
			if (dataType == NbtReader::TagLongArray) {
				dataArray = reader.ReadLongArray();
			}

			reader.SetPosition(blockStatesStart);
			reader.SkipCompound();

			// Cache decoded block states for server-side physics
			{
				std::lock_guard<std::mutex> lock(m_BlockCacheMutex);
				CacheDecodedSection(cacheKey, sectionIndex, paletteNames, dataArray,
					dataArray.empty() ? 0 : static_cast<int>(dataArray.size() * 64 / 4096));
			}

			// Now encode for the network protocol
			// Count non-air blocks
			nonAirCount = 0;
			for (const auto& name : paletteNames) {
				if (name != "minecraft:air" && name != "minecraft:cave_air" && name != "minecraft:void_air") {
					// Approximate — assume non-air blocks are present
					// Exact count would require unpacking the data array
					nonAirCount = 4096; // Conservative estimate
					break;
				}
			}

			if (paletteNames.size() <= 1) {
				// Single-valued
				output.WriteShort(paletteNames.empty() || paletteNames[0] == "minecraft:air" ? 0 : 4096);
				output.WriteShort(0);
				output.WriteByte(0); // 0 bits per entry
				output.WriteVarInt(0); // Default to air (proper mapping would use block registry)
				return;
			}

			output.WriteShort(nonAirCount);
			output.WriteShort(0); // Fluid count

			// Determine bits per entry from data array
			if (dataArray.empty()) {
				output.WriteByte(0);
				output.WriteVarInt(0);
				return;
			}

			// Calculate bits per entry: 4096 entries packed into dataArray.size() longs
			int bitsPerEntry = static_cast<int>(dataArray.size() * 64 / 4096);
			if (bitsPerEntry < 4) bitsPerEntry = 4;
			if (bitsPerEntry > 15) bitsPerEntry = 15; // Direct palette

			output.WriteByte(static_cast<uint8_t>(bitsPerEntry));

			// Write palette (using global block state IDs would require a name→ID mapping)
			// For now, write palette size 0 and use direct mode if bits >= 15
			// Actually, let's just pass through the palette indices and data as-is
			// The client's palette maps to block state IDs through the palette entries

			// We need to map palette names to block state IDs
			// For simplicity, write as direct palette (no indirect palette)
			// This means bitsPerEntry = 15, and data values ARE block state IDs

			// Actually, the simplest correct approach: if bits < 15, write an indirect
			// palette with block state IDs, then pass through the data array.
			// But we'd need a full name→stateId mapping.

			// SIMPLEST APPROACH: just pass the raw section data from the file.
			// The chunk file stores block states differently from the network format,
			// but the packing is identical (packed longs with same bit width).
			// We just need to map palette names to state IDs.

			// For now, use indirect palette with names mapped to IDs
			output.WriteVarInt(static_cast<int32_t>(paletteNames.size()));
			for (const auto& name : paletteNames) {
				output.WriteVarInt(BlockNameToStateId(name));
			}

			// Write packed data directly — the format is the same
			for (const int64_t value : dataArray) {
				output.WriteLong(value);
			}
		}

		void EncodeBiomesFromNbt(NbtReader& reader, NetworkBuffer& output) const {
			size_t biomesStart = reader.Position();

			auto paletteType = reader.FindTag("palette");
			std::vector<std::string> paletteNames;

			if (paletteType == NbtReader::TagList) {
				auto listHeader = reader.ReadListHeader();
				for (int32_t i = 0; i < listHeader.count; i++) {
					if (listHeader.elementType == NbtReader::TagString) {
						paletteNames.push_back(reader.ReadTagString());
					}
				}
			}

			reader.SetPosition(biomesStart);
			auto dataType = reader.FindTag("data");
			std::vector<int64_t> dataArray;
			if (dataType == NbtReader::TagLongArray) {
				dataArray = reader.ReadLongArray();
			}

			reader.SetPosition(biomesStart);
			reader.SkipCompound();

			if (paletteNames.size() <= 1) {
				output.WriteByte(0);
				output.WriteVarInt(paletteNames.empty() ? 0 : BiomeNameToId(paletteNames[0]));
				return;
			}

			int bitsPerEntry = 1;
			while ((1 << bitsPerEntry) < static_cast<int>(paletteNames.size())) bitsPerEntry++;
			if (bitsPerEntry > 3) bitsPerEntry = 6; // Direct biome palette

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
			if (blocks.empty()) blocks.resize(24 * 4096, 0);

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
