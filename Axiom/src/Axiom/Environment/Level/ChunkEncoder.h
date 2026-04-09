#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Environment/Level/Generator/BlockStates.h"
#include "Axiom/Network/NetworkBuffer.h"

#include <array>
#include <vector>

namespace Axiom {

	namespace Biome {
		constexpr int32_t Plains = 40;
		constexpr int32_t Desert = 14;
		constexpr int32_t Forest = 21;
		constexpr int32_t SnowyPlains = 46;
		constexpr int32_t Ocean = 35;
		constexpr int32_t Swamp = 54;
		constexpr int32_t Jungle = 28;
		constexpr int32_t Savanna = 42;
		constexpr int32_t TheVoid = 57;
	}

	namespace HeightmapType {
		constexpr int32_t WorldSurface = 1;
		constexpr int32_t MotionBlocking = 4;
	}

	class ChunkEncoder {
	public:
		// ----- Section encoding -----------------------------------------

		/**
		 * Encode a section where every block is the same type.
		 * Uses single-valued palette for both blocks and biomes.
		 */
		static bool IsFluidBlock(int32_t stateId) {
			return (stateId >= 86 && stateId <= 101)    // water states
				|| (stateId >= 102 && stateId <= 117);  // lava states
		}

		static void EncodeSingleBlockSection(NetworkBuffer& buffer, const int32_t blockStateId,
			const int32_t biomeId = Biome::Plains) {

			buffer.WriteShort(blockStateId == BlockState::Air ? 0 : 4096);
			buffer.WriteShort(IsFluidBlock(blockStateId) ? 4096 : 0);

			// Block states — single-valued palette (bits=0, no data array)
			buffer.WriteByte(0);
			buffer.WriteVarInt(blockStateId);

			// Biomes — single-valued palette (bits=0, no data array)
			buffer.WriteByte(0);
			buffer.WriteVarInt(biomeId);
		}

		/**
		 * Encode a section with per-layer block types (16 layers, each 16x16).
		 * Uses indirect palette with 4 bits per entry for blocks.
		 * Biome is single-valued for the whole section.
		 */
		static void EncodeMixedSection(NetworkBuffer& buffer, const std::array<int32_t, 16>& layers,
			const int32_t biomeId = Biome::Plains) {

			// Build palette from unique block types
			std::vector<int32_t> palette;
			std::array<int32_t, 16> paletteIndices{};
			int nonAirCount = 0;

			for (int y = 0; y < 16; y++) {
				int32_t blockState = layers[y];
				int32_t paletteIndex = -1;

				for (int32_t i = 0; i < static_cast<int32_t>(palette.size()); i++) {
					if (palette[i] == blockState) {
						paletteIndex = i;
						break;
					}
				}

				if (paletteIndex == -1) {
					paletteIndex = static_cast<int32_t>(palette.size());
					palette.push_back(blockState);
				}

				paletteIndices[y] = paletteIndex;
				if (blockState != BlockState::Air) {
					nonAirCount += 256;
				}
			}

			if (palette.size() == 1) {
				EncodeSingleBlockSection(buffer, palette[0], biomeId);
				return;
			}

			int fluidCount = 0;
			for (int y = 0; y < 16; y++) {
				if (IsFluidBlock(layers[y])) fluidCount += 256;
			}

			buffer.WriteShort(static_cast<int16_t>(nonAirCount));
			buffer.WriteShort(static_cast<int16_t>(fluidCount));

			// Block states — indirect palette, 4 bits per entry
			constexpr int bitsPerEntry = 4;
			constexpr int entriesPerLong = 64 / bitsPerEntry; // 16

			buffer.WriteByte(bitsPerEntry);

			buffer.WriteVarInt(static_cast<int32_t>(palette.size()));
			for (const int32_t blockState : palette) {
				buffer.WriteVarInt(blockState);
			}

			// Pack 4096 entries into longs
			constexpr int totalEntries = 4096;
			constexpr int longCount = totalEntries / entriesPerLong; // 256

			for (int longIndex = 0; longIndex < longCount; longIndex++) {
				int64_t packed = 0;
				const int startEntry = longIndex * entriesPerLong;

				for (int offset = 0; offset < entriesPerLong; offset++) {
					const int entryIndex = startEntry + offset;
					const int layer = entryIndex / 256;
					const int32_t index = paletteIndices[layer];
					packed |= (static_cast<int64_t>(index) << (offset * bitsPerEntry));
				}

				buffer.WriteLong(packed);
			}

			// Biomes — single-valued (bits=0, no data array)
			buffer.WriteByte(0);
			buffer.WriteVarInt(biomeId);
		}

		/**
		 * Encode a section with per-block state IDs (4096 entries).
		 * blocks must contain exactly 4096 entries in YZX order.
		 * Uses indirect palette with computed bits per entry.
		 */
		static void EncodeBlockSection(NetworkBuffer& buffer,
			const int32_t* blocks, const int32_t biomeId = Biome::Plains) {

			// Build palette from unique block types (hash map for O(1) lookup)
			std::vector<int32_t> palette;
			std::unordered_map<int32_t, int32_t> paletteMap;
			std::array<int32_t, 4096> paletteIndices{};
			int nonAirCount = 0;
			int fluidCount = 0;

			for (int index = 0; index < 4096; index++) {
				int32_t blockState = blocks[index];

				auto iterator = paletteMap.find(blockState);
				if (iterator == paletteMap.end()) {
					int32_t paletteIndex = static_cast<int32_t>(palette.size());
					palette.push_back(blockState);
					paletteMap[blockState] = paletteIndex;
					paletteIndices[index] = paletteIndex;
				} else {
					paletteIndices[index] = iterator->second;
				}

				if (blockState != BlockState::Air) {
					nonAirCount++;
				}
				if (IsFluidBlock(blockState)) {
					fluidCount++;
				}
			}

			if (palette.size() == 1) {
				EncodeSingleBlockSection(buffer, palette[0], biomeId);
				return;
			}

			buffer.WriteShort(static_cast<int16_t>(nonAirCount));
			buffer.WriteShort(static_cast<int16_t>(fluidCount));

			// Compute bits per entry (minimum 4 for indirect palette)
			int bitsPerEntry = 4;
			while ((1 << bitsPerEntry) < static_cast<int>(palette.size())) {
				bitsPerEntry++;
			}

			const int entriesPerLong = 64 / bitsPerEntry;

			buffer.WriteByte(static_cast<uint8_t>(bitsPerEntry));

			buffer.WriteVarInt(static_cast<int32_t>(palette.size()));
			for (const int32_t blockState : palette) {
				buffer.WriteVarInt(blockState);
			}

			// Pack 4096 entries into longs
			constexpr int totalEntries = 4096;
			const int longCount = (totalEntries + entriesPerLong - 1) / entriesPerLong;

			for (int longIndex = 0; longIndex < longCount; longIndex++) {
				int64_t packed = 0;
				const int startEntry = longIndex * entriesPerLong;

				for (int offset = 0; offset < entriesPerLong && (startEntry + offset) < totalEntries; offset++) {
					const int entryIndex = startEntry + offset;
					const int32_t index = paletteIndices[entryIndex];
					packed |= (static_cast<int64_t>(index) << (offset * bitsPerEntry));
				}

				buffer.WriteLong(packed);
			}

			// Biomes — single-valued (bits=0, no data array)
			buffer.WriteByte(0);
			buffer.WriteVarInt(biomeId);
		}

		// ----- Superflat chunk ------------------------------------------

		static std::vector<uint8_t> EncodeSuperflat(const int32_t biomeId = Biome::Plains) {
			NetworkBuffer chunkData;

			// Section 0 (y=-64 to -49): bedrock + stone
			std::array<int32_t, 16> bottomLayers{};
			bottomLayers[0] = BlockState::Bedrock;
			for (int i = 1; i < 16; i++) {
				bottomLayers[i] = BlockState::Stone;
			}
			EncodeMixedSection(chunkData, bottomLayers, biomeId);

			// Section 1 (y=-48 to -33): stone + water on top 3 layers
			std::array<int32_t, 16> midLayers{};
			for (int i = 0; i < 13; i++) {
				midLayers[i] = BlockState::Stone;
			}
			for (int i = 13; i < 16; i++) {
				midLayers[i] = BlockState::Water;
			}
			EncodeMixedSection(chunkData, midLayers, biomeId);

			// Sections 2-23: all air
			for (int section = 2; section < 24; section++) {
				EncodeSingleBlockSection(chunkData, BlockState::Air, biomeId);
			}

			return std::move(chunkData.Data());
		}

		// ----- Heightmaps -----------------------------------------------

		static void EncodeHeightmaps(NetworkBuffer& buffer, const int32_t heightValue) {
			buffer.WriteVarInt(2);

			constexpr int bitsPerEntry = 9;
			constexpr int entriesPerLong = 64 / bitsPerEntry;
			constexpr int longCount = (256 + entriesPerLong - 1) / entriesPerLong;

			std::vector<int64_t> packedData(longCount, 0);
			for (int entry = 0; entry < 256; entry++) {
				const int longIndex = entry / entriesPerLong;
				const int bitOffset = (entry % entriesPerLong) * bitsPerEntry;
				packedData[longIndex] |= (static_cast<int64_t>(heightValue) << bitOffset);
			}

			buffer.WriteVarInt(HeightmapType::MotionBlocking);
			buffer.WriteVarInt(static_cast<int32_t>(packedData.size()));
			for (const int64_t value : packedData) {
				buffer.WriteLong(value);
			}

			buffer.WriteVarInt(HeightmapType::WorldSurface);
			buffer.WriteVarInt(static_cast<int32_t>(packedData.size()));
			for (const int64_t value : packedData) {
				buffer.WriteLong(value);
			}
		}

		// ----- Light data -----------------------------------------------

		static std::vector<uint8_t> MakeLightArray(const uint8_t lightLevel) {
			const uint8_t packed = static_cast<uint8_t>((lightLevel & 0xF) | ((lightLevel & 0xF) << 4));
			return std::vector<uint8_t>(2048, packed);
		}

		static std::vector<uint8_t> MakeBottomSectionSkyLight() {
			std::vector<uint8_t> data(2048, 0);
			for (int layer = 4; layer < 16; layer++) {
				const int byteOffset = layer * 128;
				for (int i = 0; i < 128; i++) {
					data[byteOffset + i] = 0xFF;
				}
			}
			return data;
		}

		static void WriteBitSet(NetworkBuffer& buffer, const std::vector<int>& setBits) {
			if (setBits.empty()) {
				buffer.WriteVarInt(0);
				return;
			}

			int maxBit = 0;
			for (const int bit : setBits) {
				if (bit > maxBit) maxBit = bit;
			}

			const int longCount = (maxBit / 64) + 1;
			std::vector<int64_t> longs(longCount, 0);

			for (const int bit : setBits) {
				longs[bit / 64] |= (1LL << (bit % 64));
			}

			buffer.WriteVarInt(longCount);
			for (const int64_t value : longs) {
				buffer.WriteLong(value);
			}
		}

		static void EncodeLightData(NetworkBuffer& buffer) {
			std::vector<int> skyLightSections;
			for (int i = 1; i <= 25; i++) {
				skyLightSections.push_back(i);
			}

			std::vector<int> blockLightSections;
			std::vector<int> emptySkyLight = {0};

			std::vector<int> emptyBlockLight;
			for (int i = 0; i <= 25; i++) {
				emptyBlockLight.push_back(i);
			}

			WriteBitSet(buffer, skyLightSections);
			WriteBitSet(buffer, blockLightSections);
			WriteBitSet(buffer, emptySkyLight);
			WriteBitSet(buffer, emptyBlockLight);

			buffer.WriteVarInt(static_cast<int32_t>(skyLightSections.size()));
			for (int sectionIndex : skyLightSections) {
				std::vector<uint8_t> lightArray;
				if (sectionIndex == 1) {
					lightArray = MakeBottomSectionSkyLight();
				} else {
					lightArray = MakeLightArray(15);
				}
				buffer.WriteVarInt(static_cast<int32_t>(lightArray.size()));
				buffer.WriteBytes(lightArray);
			}

			buffer.WriteVarInt(0); // No block light arrays
		}

		// ----- Full chunk packet ----------------------------------------

		static void EncodeLevelChunkWithLight(NetworkBuffer& packet, const int32_t chunkX, const int32_t chunkZ,
			const int32_t biomeId = Biome::Plains) {

			packet.WriteInt(chunkX);
			packet.WriteInt(chunkZ);

			EncodeHeightmaps(packet, 4);

			const auto chunkData = EncodeSuperflat(biomeId);
			packet.WriteVarInt(static_cast<int32_t>(chunkData.size()));
			packet.WriteBytes(chunkData);

			packet.WriteVarInt(0); // Block entities

			EncodeLightData(packet);
		}
	};

}
