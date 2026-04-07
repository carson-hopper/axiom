#pragma once

#include "Axiom/Environment/World/Generator/ChunkGenerator.h"
#include "Axiom/Environment/World/Generator/Noise.h"
#include "Axiom/Environment/World/ChunkEncoder.h"

#include <algorithm>
#include <array>
#include <cmath>

namespace Axiom {

	namespace BlockState {
		constexpr int32_t Water = 86;
		constexpr int32_t Sand = 118;
		constexpr int32_t Gravel = 124;
		constexpr int32_t Sandstone = 578;
		constexpr int32_t Deepslate = 27924;
	}

	/**
	 * Generates natural-looking terrain using layered Perlin noise.
	 * Produces rolling hills, oceans, beaches, and underground layers.
	 */
	class TerrainChunkGenerator : public ChunkGenerator {
	public:
		static constexpr int SeaLevel = 62;
		static constexpr int MinY = -64;
		static constexpr int MaxY = 319;

		explicit TerrainChunkGenerator(uint64_t seed = 12345)
			: m_TerrainNoise(seed)
			, m_DetailNoise(seed + 1)
			, m_ContinentNoise(seed + 2)
			, m_ErosionNoise(seed + 3) {}

		ChunkData Generate(int32_t chunkX, int32_t chunkZ) override {
			NetworkBuffer sectionData;
			int maxHeight = MinY;

			// Generate heightmap for this chunk (16x16)
			std::array<int, 256> heightmap{};
			for (int localZ = 0; localZ < 16; localZ++) {
				for (int localX = 0; localX < 16; localX++) {
					int worldX = chunkX * 16 + localX;
					int worldZ = chunkZ * 16 + localZ;
					int height = CalculateHeight(worldX, worldZ);
					heightmap[localZ * 16 + localX] = height;
					if (height > maxHeight) maxHeight = height;
				}
			}

			// Encode 24 sections
			for (int sectionIndex = 0; sectionIndex < 24; sectionIndex++) {
				int sectionMinY = MinY + sectionIndex * 16;
				int sectionMaxY = sectionMinY + 15;

				// Check if this section has any non-air blocks
				bool hasBlocks = false;
				for (int i = 0; i < 256 && !hasBlocks; i++) {
					int height = heightmap[i];
					// Section has blocks if terrain or water reaches into it
					if (height >= sectionMinY || (SeaLevel >= sectionMinY && sectionMinY <= SeaLevel)) {
						hasBlocks = true;
					}
				}

				if (!hasBlocks && sectionMinY > SeaLevel) {
					// Fully air section
					ChunkEncoder::EncodeSingleBlockSection(sectionData, BlockState::Air, Biome::Plains);
					continue;
				}

				// Build per-block palette for this section
				EncodeTerrainSection(sectionData, chunkX, chunkZ, sectionMinY, heightmap);
			}

			// Calculate heightmap value for the packet (max solid block height above MinY)
			int heightmapValue = std::max(0, maxHeight - MinY + 1);
			// Clamp — also account for sea level
			heightmapValue = std::max(heightmapValue, SeaLevel - MinY + 1);

			return ChunkData{
				chunkX,
				chunkZ,
				std::move(sectionData.Data()),
				heightmapValue,
				Biome::Plains  // TODO: biome selection
			};
		}

		double SpawnY() const override {
			return static_cast<double>(CalculateHeight(0, 0)) + 1.0;
		}

	private:
		int CalculateHeight(int worldX, int worldZ) const {
			double x = static_cast<double>(worldX);
			double z = static_cast<double>(worldZ);

			// Continental noise — large-scale land vs ocean
			double continent = m_ContinentNoise.OctaveNoise(x * 0.001, z * 0.001, 4, 0.5);

			// Erosion — flattens peaks, creates valleys
			double erosion = m_ErosionNoise.OctaveNoise(x * 0.002, z * 0.002, 3, 0.5);

			// Main terrain shape
			double terrain = m_TerrainNoise.OctaveNoise(x * 0.008, z * 0.008, 6, 0.5);

			// Fine detail
			double detail = m_DetailNoise.OctaveNoise(x * 0.03, z * 0.03, 3, 0.4);

			// Combine: continent controls land/ocean, terrain adds hills
			double baseHeight = 64.0;  // Sea level area

			// Continental factor: positive = land, negative = ocean
			double continentFactor = continent * 40.0;

			// Terrain adds hills/mountains on land
			double terrainHeight = terrain * 20.0 * std::max(0.0, 0.5 + continent);

			// Erosion smooths terrain
			double erosionFactor = 1.0 - std::abs(erosion) * 0.5;
			terrainHeight *= erosionFactor;

			// Detail adds small bumps
			double detailHeight = detail * 4.0;

			double finalHeight = baseHeight + continentFactor + terrainHeight + detailHeight;

			// Clamp to valid range
			return std::clamp(static_cast<int>(finalHeight), MinY + 1, MaxY - 1);
		}

		int32_t GetBlockAt(int worldX, int worldY, int worldZ, int surfaceHeight) const {
			if (worldY > surfaceHeight) {
				// Above terrain
				if (worldY <= SeaLevel) {
					return BlockState::Water;
				}
				return BlockState::Air;
			}

			if (worldY == surfaceHeight) {
				// Surface block
				if (surfaceHeight <= SeaLevel + 1 && surfaceHeight >= SeaLevel - 2) {
					return BlockState::Sand;  // Beach
				}
				if (surfaceHeight <= SeaLevel - 3) {
					return BlockState::Gravel;  // Ocean floor
				}
				return BlockState::GrassBlock;  // Normal land
			}

			if (worldY > surfaceHeight - 4) {
				// Sub-surface (3 blocks deep)
				if (surfaceHeight <= SeaLevel + 1) {
					return BlockState::Sand;
				}
				return BlockState::Dirt;
			}

			if (worldY <= 0) {
				// Deep underground
				return BlockState::Deepslate;
			}

			if (worldY <= MinY + 4) {
				// Bedrock layer (with some randomness)
				return BlockState::Bedrock;
			}

			return BlockState::Stone;
		}

		void EncodeTerrainSection(NetworkBuffer& buffer, int32_t chunkX, int32_t chunkZ,
			int sectionMinY, const std::array<int, 256>& heightmap) const {

			// Collect unique block states and build palette
			std::vector<int32_t> palette;
			std::array<int32_t, 4096> blockIndices{};
			int nonAirCount = 0;

			auto getPaletteIndex = [&palette](int32_t stateId) -> int32_t {
				for (int32_t i = 0; i < static_cast<int32_t>(palette.size()); i++) {
					if (palette[i] == stateId) return i;
				}
				palette.push_back(stateId);
				return static_cast<int32_t>(palette.size()) - 1;
			};

			for (int localY = 0; localY < 16; localY++) {
				int worldY = sectionMinY + localY;
				for (int localZ = 0; localZ < 16; localZ++) {
					for (int localX = 0; localX < 16; localX++) {
						int surfaceHeight = heightmap[localZ * 16 + localX];
						int worldX = chunkX * 16 + localX;
						int worldZ = chunkZ * 16 + localZ;

						int32_t blockState = GetBlockAt(worldX, worldY, worldZ, surfaceHeight);
						int index = localY * 256 + localZ * 16 + localX;
						blockIndices[index] = getPaletteIndex(blockState);

						if (blockState != BlockState::Air) {
							nonAirCount++;
						}
					}
				}
			}

			// Single block type optimization
			if (palette.size() == 1) {
				ChunkEncoder::EncodeSingleBlockSection(buffer, palette[0], Biome::Plains);
				return;
			}

			buffer.WriteShort(static_cast<int16_t>(nonAirCount));
			buffer.WriteShort(0); // Fluid count

			// Determine bits per entry (minimum 4 for blocks)
			int bitsPerEntry = 4;
			while ((1 << bitsPerEntry) < static_cast<int>(palette.size())) {
				bitsPerEntry++;
			}

			buffer.WriteByte(static_cast<uint8_t>(bitsPerEntry));

			// Write palette
			buffer.WriteVarInt(static_cast<int32_t>(palette.size()));
			for (int32_t stateId : palette) {
				buffer.WriteVarInt(stateId);
			}

			// Pack entries into longs
			int entriesPerLong = 64 / bitsPerEntry;
			int longCount = (4096 + entriesPerLong - 1) / entriesPerLong;

			for (int longIndex = 0; longIndex < longCount; longIndex++) {
				int64_t packed = 0;
				int startEntry = longIndex * entriesPerLong;

				for (int offset = 0; offset < entriesPerLong; offset++) {
					int entryIndex = startEntry + offset;
					if (entryIndex >= 4096) break;
					packed |= (static_cast<int64_t>(blockIndices[entryIndex]) << (offset * bitsPerEntry));
				}

				buffer.WriteLong(packed);
			}

			// Biomes — single-valued (plains for now)
			buffer.WriteByte(0);
			buffer.WriteVarInt(Biome::Plains);
		}

		PerlinNoise m_TerrainNoise;
		PerlinNoise m_DetailNoise;
		PerlinNoise m_ContinentNoise;
		PerlinNoise m_ErosionNoise;
	};

}
