#include "axpch.h"
#include "Axiom/Environment/Level/Generator/ClassicLevelSource.h"

#include "Axiom/Environment/Level/Generator/BlockStates.h"
#include "Axiom/Environment/Level/ChunkEncoder.h"
#include "Axiom/Environment/Level/Physics/BlockPhysics.h"
#include "Axiom/Network/NetworkBuffer.h"

#include <algorithm>
#include <cmath>

namespace Axiom {

	/**
	 * Map classic BiomeRegistry IDs to the modern
	 * network biome registry IDs used by the client.
	 */
	static int32_t MapBiomeToNetwork(int32_t classicId) {
		switch (classicId) {
			case BiomeRegistry::OCEAN:          return Biome::Ocean;
			case BiomeRegistry::PLAINS:         return Biome::Plains;
			case BiomeRegistry::DESERT:         return Biome::Desert;
			case BiomeRegistry::MOUNTAINS:      return Biome::Plains;
			case BiomeRegistry::FOREST:         return Biome::Forest;
			case BiomeRegistry::TAIGA:          return Biome::SnowyPlains;
			case BiomeRegistry::SWAMP:          return Biome::Swamp;
			case BiomeRegistry::RIVER:          return Biome::Plains;
			case BiomeRegistry::FROZEN_OCEAN:   return Biome::Ocean;
			case BiomeRegistry::ICE_PLAINS:     return Biome::SnowyPlains;
			case BiomeRegistry::MUSHROOM_ISLAND:return Biome::Plains;
			case BiomeRegistry::BEACH:          return Biome::Desert;
			case BiomeRegistry::JUNGLE:         return Biome::Jungle;
			case BiomeRegistry::SAVANNA:        return Biome::Savanna;
			case BiomeRegistry::BADLANDS:       return Biome::Desert;
			default:                            return Biome::Plains;
		}
	}

	ClassicLevelSource::ClassicLevelSource(int64_t seed)
		: m_Seed(seed)
		, m_NoiseMain(seed)
		, m_NoiseDetail(seed * 6364136223846793005LL + 1)
		, m_NoiseSurface(seed * 1442695040888963407LL + 7)
		, m_BiomeSource(seed) {

		BiomeRegistry::Bootstrap();
	}

	ChunkData ClassicLevelSource::Generate(int32_t chunkX, int32_t chunkZ) {
		std::vector<int32_t> blocks(WORLD_HEIGHT * 16 * 16, BlockState::Air);
		std::vector<int32_t> biomeGrid = m_BiomeSource.GetBiomes(chunkX * 16, chunkZ * 16, 16, 16);

		PrepareHeights(chunkX, chunkZ, blocks, biomeGrid);
		BuildSurfaces(chunkX, chunkZ, blocks, biomeGrid);
		m_CaveGenerator.Generate(m_Seed, chunkX, chunkZ, blocks);

		// Fill caves under water: any air block with water directly
		// above gets replaced with water to prevent floating oceans
		for (int localZ = 0; localZ < 16; localZ++) {
			for (int localX = 0; localX < 16; localX++) {
				for (int worldY = SEA_LEVEL; worldY >= 1; worldY--) {
					int index = worldY * 256 + localZ * 16 + localX;
					int aboveIndex = (worldY + 1) * 256 + localZ * 16 + localX;
					if (blocks[index] == BlockState::Air &&
						(blocks[aboveIndex] == BlockState::Water || FluidState::IsFluid(blocks[aboveIndex]))) {
						blocks[index] = BlockState::Water;
					}
				}
			}
		}

		m_FeatureDecorator.Decorate(m_Seed, chunkX, chunkZ, blocks, biomeGrid);

		CacheBlocks(chunkX, chunkZ, blocks);

		auto chunkData = EncodeChunk(chunkX, chunkZ, blocks, biomeGrid);
		chunkData.skyLight = ComputeSkyLight(blocks);
		return chunkData;
	}

	double ClassicLevelSource::SpawnY() const {
		return static_cast<double>(SEA_LEVEL + 4);
	}

	int32_t ClassicLevelSource::GetBlockAt(int32_t worldX, int32_t worldY, int32_t worldZ) const {
		const int32_t chunkX = worldX >> 4;
		const int32_t chunkZ = worldZ >> 4;
		const int64_t key = CacheKey(chunkX, chunkZ);

		std::lock_guard<std::mutex> lock(m_CacheMutex);
		const auto iterator = m_BlockCache.find(key);
		if (iterator == m_BlockCache.end()) {
			return BlockState::Air;
		}

		if (worldY < 0 || worldY >= WORLD_HEIGHT) {
			return BlockState::Air;
		}

		const int localX = worldX & 0xF;
		const int localZ = worldZ & 0xF;
		const int blockIndex = worldY * 256 + localZ * 16 + localX;
		return iterator->second[blockIndex];
	}

	void ClassicLevelSource::PrepareHeights(int chunkX, int chunkZ,
		std::vector<int32_t>& blocks,
		std::vector<int32_t>& biomeGrid) const {

		for (int localZ = 0; localZ < 16; localZ++) {
			for (int localX = 0; localX < 16; localX++) {
				const int worldX = chunkX * 16 + localX;
				const int worldZ = chunkZ * 16 + localZ;

				// Biome blending: sample a 5x5 area and average depth/scale
				// to smooth transitions between biomes
				float blendedDepth = 0.0f;
				float blendedScale = 0.0f;
				float totalWeight = 0.0f;

				for (int blendZ = -2; blendZ <= 2; blendZ++) {
					for (int blendX = -2; blendX <= 2; blendX++) {
						int sampleX = localX + blendX;
						int sampleZ = localZ + blendZ;

						float depth = 0.125f;
						float scale = 0.05f;

						if (sampleX >= 0 && sampleX < 16 && sampleZ >= 0 && sampleZ < 16) {
							int32_t sampleBiome = biomeGrid[sampleZ * 16 + sampleX];
							const BiomeData* biome = BiomeRegistry::Get(sampleBiome);
							if (biome) {
								depth = biome->Depth;
								scale = biome->Scale;
							}
						} else {
							// Use this column's biome for out-of-chunk samples
							const BiomeData* biome = BiomeRegistry::Get(biomeGrid[localZ * 16 + localX]);
							if (biome) {
								depth = biome->Depth;
								scale = biome->Scale;
							}
						}

						// Closer samples get more weight
						float distance = std::sqrt(static_cast<float>(blendX * blendX + blendZ * blendZ));
						float weight = 1.0f / (1.0f + distance);
						blendedDepth += depth * weight;
						blendedScale += scale * weight;
						totalWeight += weight;
					}
				}

				blendedDepth /= totalWeight;
				blendedScale /= totalWeight;

				double mainNoise = m_NoiseMain.OctaveNoise2D(
					worldX * 0.01, worldZ * 0.01, 8);
				double detailNoise = m_NoiseDetail.OctaveNoise2D(
					worldX * 0.05, worldZ * 0.05, 4);

				double rawHeight = 64.0 + blendedDepth * 17.0
					+ mainNoise * 12.0 * (blendedScale + 0.2)
					+ detailNoise * 3.0;

				int height = std::clamp(static_cast<int>(rawHeight), 1, 255);

				// Bedrock at y=0
				blocks[0 * 256 + localZ * 16 + localX] = BlockState::Bedrock;

				// Stone from y=1 to height
				for (int blockY = 1; blockY <= height; blockY++) {
					blocks[blockY * 256 + localZ * 16 + localX] = BlockState::Stone;
				}

				// Water from height+1 to SEA_LEVEL if below sea level
				if (height < SEA_LEVEL) {
					for (int blockY = height + 1; blockY <= SEA_LEVEL; blockY++) {
						blocks[blockY * 256 + localZ * 16 + localX] = BlockState::Water;
					}
				}
			}
		}
	}

	void ClassicLevelSource::BuildSurfaces(int chunkX, int chunkZ,
		std::vector<int32_t>& blocks,
		const std::vector<int32_t>& biomeGrid) const {

		for (int localZ = 0; localZ < 16; localZ++) {
			for (int localX = 0; localX < 16; localX++) {
				const int worldX = chunkX * 16 + localX;
				const int worldZ = chunkZ * 16 + localZ;

				const int biomeIndex = localZ * 16 + localX;
				const int32_t biomeId = biomeGrid[biomeIndex];
				const BiomeData* biome = BiomeRegistry::Get(biomeId);

				int32_t topBlock = BlockState::GrassBlock;
				int32_t fillerBlock = BlockState::Dirt;
				if (biome) {
					topBlock = biome->TopBlock;
					fillerBlock = biome->FillerBlock;
				}

				double surfaceNoise = m_NoiseSurface.OctaveNoise2D(
					worldX * 0.0625, worldZ * 0.0625, 4);
				int surfaceDepth = 3 + static_cast<int>(surfaceNoise * 2.0);
				if (surfaceDepth < 1) {
					surfaceDepth = 1;
				}

				/**
				 * Scan downward from the top of the
				 * world to find the terrain surface.
				 */
				int replacedCount = 0;
				for (int blockY = WORLD_HEIGHT - 1; blockY >= 1; blockY--) {
					int blockIndex = blockY * 256 + localZ * 16 + localX;
					int32_t currentBlock = blocks[blockIndex];

					if (currentBlock != BlockState::Stone) {
						continue;
					}

					/**
					 * Only apply surface blocks above
					 * water to avoid underwater grass.
					 */
					if (replacedCount == 0) {
						int aboveIndex = (blockY + 1) * 256 + localZ * 16 + localX;
						int32_t aboveBlock = (blockY + 1 < WORLD_HEIGHT)
							? blocks[aboveIndex] : BlockState::Air;

						bool nearWater = (aboveBlock == BlockState::Water);

						// Beach: use sand near the waterline
						if (!nearWater && blockY >= SEA_LEVEL - 1 && blockY <= SEA_LEVEL + 2) {
							for (int checkY = blockY + 1; checkY <= SEA_LEVEL + 1 && checkY < WORLD_HEIGHT; checkY++) {
								if (blocks[checkY * 256 + localZ * 16 + localX] == BlockState::Water) {
									nearWater = true;
									break;
								}
							}
						}

						if (nearWater) {
							blocks[blockIndex] = BlockState::Sand;
							fillerBlock = BlockState::Sand;
						} else {
							blocks[blockIndex] = topBlock;
						}
						replacedCount++;
					} else if (replacedCount < surfaceDepth) {
						blocks[blockIndex] = fillerBlock;
						replacedCount++;
					} else {
						break;
					}
				}
			}
		}
	}

	ChunkData ClassicLevelSource::EncodeChunk(int chunkX, int chunkZ,
		const std::vector<int32_t>& blocks,
		const std::vector<int32_t>& biomeGrid) const {

		NetworkBuffer sectionBuffer;

		/**
		 * Pick a representative network biome for the
		 * chunk from the center column of the grid.
		 */
		int32_t dominantBiome = MapBiomeToNetwork(biomeGrid[8 * 16 + 8]);

		/**
		 * Encode 24 sections covering y -64 to 319.
		 * Section 0 = y -64..-49, section 4 = y 0..15, etc.
		 * Our block array covers y 0..255, so only sections
		 * 4 through 19 can contain generated blocks.
		 */
		for (int section = 0; section < 24; section++) {
			int baseWorldY = (section * 16) - 64;

			/**
			 * Sections entirely below y=0 or above y=255
			 * contain no generated blocks, encode as air.
			 */
			if (baseWorldY < 0 || baseWorldY >= WORLD_HEIGHT) {
				ChunkEncoder::EncodeSingleBlockSection(sectionBuffer, BlockState::Air, dominantBiome);
				continue;
			}

			std::array<int32_t, 4096> sectionBlocks{};

			for (int localY = 0; localY < 16; localY++) {
				int worldY = baseWorldY + localY;
				for (int localZ = 0; localZ < 16; localZ++) {
					for (int localX = 0; localX < 16; localX++) {
						int sourceIndex = worldY * 256 + localZ * 16 + localX;
						int destIndex = (localY * 16 + localZ) * 16 + localX;
						sectionBlocks[destIndex] = blocks[sourceIndex];
					}
				}
			}

			ChunkEncoder::EncodeBlockSection(sectionBuffer, sectionBlocks.data(), dominantBiome);
		}

		/**
		 * Compute a representative heightmap value from the
		 * center column, offset by 64 for the protocol.
		 */
		int heightmapValue = 0;
		for (int blockY = WORLD_HEIGHT - 1; blockY >= 0; blockY--) {
			if (blocks[blockY * 256 + 8 * 16 + 8] != BlockState::Air) {
				heightmapValue = blockY + 64 + 1;
				break;
			}
		}

		return ChunkData{
			chunkX, chunkZ,
			std::move(sectionBuffer.Data()),
			heightmapValue,
			dominantBiome
		};
	}

	static bool IsOpaqueBlock(int32_t state) {
		return state != BlockState::Air
			&& !FluidState::IsFluid(state)
			&& state != BlockState::OakLeaves
			&& state != BlockState::SpruceLeaves
			&& state != BlockState::BirchLeaves
			&& state != BlockState::ShortGrass
			&& state != BlockState::Fern
			&& state != BlockState::Dandelion
			&& state != BlockState::Poppy;
	}

	std::vector<std::vector<uint8_t>> ClassicLevelSource::ComputeSkyLight(
		const std::vector<int32_t>& blocks) const {

		// 3D light volume: light[y * 256 + z * 16 + x]
		std::vector<uint8_t> light(WORLD_HEIGHT * 16 * 16, 0);

		// Phase 1: seed sky light from above — full brightness above the
		// highest opaque block in each column, trace straight down
		for (int localZ = 0; localZ < 16; localZ++) {
			for (int localX = 0; localX < 16; localX++) {
				uint8_t currentLight = 15;
				for (int worldY = WORLD_HEIGHT - 1; worldY >= 0; worldY--) {
					int32_t state = blocks[worldY * 256 + localZ * 16 + localX];
					if (IsOpaqueBlock(state)) {
						currentLight = 0;
					}
					light[worldY * 256 + localZ * 16 + localX] = currentLight;
				}
			}
		}

		// Phase 2: flood-fill horizontal propagation
		// Light spreads into adjacent non-opaque blocks, losing 1 per step.
		// Multiple passes until stable (max 15 passes for full spread).
		static constexpr int DirectionX[] = {1, -1, 0, 0};
		static constexpr int DirectionZ[] = {0, 0, 1, -1};

		for (int pass = 0; pass < 15; pass++) {
			bool changed = false;
			for (int worldY = 0; worldY < WORLD_HEIGHT; worldY++) {
				for (int localZ = 0; localZ < 16; localZ++) {
					for (int localX = 0; localX < 16; localX++) {
						int index = worldY * 256 + localZ * 16 + localX;
						uint8_t current = light[index];
						if (current <= 1) continue;

						// Spread to 4 horizontal neighbors
						for (int direction = 0; direction < 4; direction++) {
							int neighborX = localX + DirectionX[direction];
							int neighborZ = localZ + DirectionZ[direction];
							if (neighborX < 0 || neighborX >= 16 ||
								neighborZ < 0 || neighborZ >= 16) continue;

							int neighborIndex = worldY * 256 + neighborZ * 16 + neighborX;
							if (IsOpaqueBlock(blocks[neighborIndex])) continue;

							uint8_t spread = current - 1;
							if (spread > light[neighborIndex]) {
								light[neighborIndex] = spread;
								changed = true;
							}
						}

						// Spread down (light falls through air)
						if (worldY > 0) {
							int belowIndex = (worldY - 1) * 256 + localZ * 16 + localX;
							if (!IsOpaqueBlock(blocks[belowIndex])) {
								uint8_t spread = current - 1;
								if (spread > light[belowIndex]) {
									light[belowIndex] = spread;
									changed = true;
								}
							}
						}
					}
				}
			}
			if (!changed) break;
		}

		// Phase 3: pack into per-section nibble arrays
		std::vector<std::vector<uint8_t>> result(24);

		for (int section = 0; section < 24; section++) {
			int baseWorldY = section * 16 - 64;
			result[section].resize(2048, 0);
			auto& lightData = result[section];

			for (int localY = 0; localY < 16; localY++) {
				int worldY = baseWorldY + localY;
				for (int localZ = 0; localZ < 16; localZ++) {
					for (int localX = 0; localX < 16; localX++) {
						int blockIndex = (localY * 16 + localZ) * 16 + localX;

						uint8_t skyLevel;
						if (worldY < 0) {
							skyLevel = 0;
						} else if (worldY >= WORLD_HEIGHT) {
							skyLevel = 15;
						} else {
							skyLevel = light[worldY * 256 + localZ * 16 + localX];
						}

						int byteIndex = blockIndex / 2;
						if (blockIndex % 2 == 0) {
							lightData[byteIndex] |= (skyLevel & 0xF);
						} else {
							lightData[byteIndex] |= ((skyLevel & 0xF) << 4);
						}
					}
				}
			}
		}

		return result;
	}

	void ClassicLevelSource::CacheBlocks(int chunkX, int chunkZ,
		const std::vector<int32_t>& blocks) const {

		const int64_t key = CacheKey(chunkX, chunkZ);
		std::lock_guard<std::mutex> lock(m_CacheMutex);
		m_BlockCache[key] = blocks;
	}

}
