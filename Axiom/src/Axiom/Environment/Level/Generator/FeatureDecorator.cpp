#include "Axiom/Environment/Level/Generator/FeatureDecorator.h"
#include "Axiom/Environment/Level/Generator/Biome.h"
#include "Axiom/Environment/Level/Generator/BlockStates.h"

#include <algorithm>
#include <cmath>
#include <random>

namespace Axiom {

	void FeatureDecorator::Decorate(int64_t worldSeed, int chunkX, int chunkZ,
		std::vector<int32_t>& blocks,
		const std::vector<int32_t>& biomeGrid) const {

		uint64_t decoratorSeed = static_cast<uint64_t>(worldSeed) ^
			(static_cast<uint64_t>(chunkX) * 341873128712ULL) ^
			(static_cast<uint64_t>(chunkZ) * 132897987541ULL);

		PlaceOres(decoratorSeed, chunkX, chunkZ, blocks);
		PlaceTrees(decoratorSeed + 1, chunkX, chunkZ, blocks, biomeGrid);
		PlaceVegetation(decoratorSeed + 2, chunkX, chunkZ, blocks, biomeGrid);
	}

	void FeatureDecorator::PlaceOres(uint64_t seed, int chunkX, int chunkZ,
		std::vector<int32_t>& blocks) const {

		std::mt19937_64 random(seed);

		// Coal: 20 veins, size 17, y 0-128
		for (int vein = 0; vein < 20; vein++) {
			PlaceOreVein(blocks, random(), BlockState::CoalOre, 17,
				chunkX, chunkZ, 0, 128);
		}

		// Iron: 20 veins, size 9, y 0-64
		for (int vein = 0; vein < 20; vein++) {
			PlaceOreVein(blocks, random(), BlockState::IronOre, 9,
				chunkX, chunkZ, 0, 64);
		}

		// Gold: 2 veins, size 9, y 0-32
		for (int vein = 0; vein < 2; vein++) {
			PlaceOreVein(blocks, random(), BlockState::GoldOre, 9,
				chunkX, chunkZ, 0, 32);
		}

		// Redstone: 8 veins, size 8, y 0-16
		for (int vein = 0; vein < 8; vein++) {
			PlaceOreVein(blocks, random(), BlockState::RedstoneOre, 8,
				chunkX, chunkZ, 0, 16);
		}

		// Diamond: 1 vein, size 8, y 0-16
		PlaceOreVein(blocks, random(), BlockState::DiamondOre, 8,
			chunkX, chunkZ, 0, 16);

		// Lapis: 1 vein, size 7, y 0-32
		PlaceOreVein(blocks, random(), BlockState::LapisOre, 7,
			chunkX, chunkZ, 0, 32);
	}

	void FeatureDecorator::PlaceOreVein(std::vector<int32_t>& blocks, uint64_t seed,
		int32_t oreBlock, int veinSize, int chunkX, int chunkZ,
		int minY, int maxY) const {

		std::mt19937_64 random(seed);

		int yRange = maxY - minY;
		if (yRange <= 0) return;

		// Random position within the chunk
		int localX = static_cast<int>(random() % 16);
		int localZ = static_cast<int>(random() % 16);
		int blockY = static_cast<int>(random() % yRange) + minY;

		// Generate the vein as a spheroid cluster
		float veinAngle = static_cast<float>(random() % 628) / 100.0f;
		int worldX = chunkX * 16 + localX;
		int worldZ = chunkZ * 16 + localZ;

		double startX = static_cast<double>(worldX) +
			std::sin(veinAngle) * static_cast<double>(veinSize) / 8.0;
		double endX = static_cast<double>(worldX) -
			std::sin(veinAngle) * static_cast<double>(veinSize) / 8.0;
		double startZ = static_cast<double>(worldZ) +
			std::cos(veinAngle) * static_cast<double>(veinSize) / 8.0;
		double endZ = static_cast<double>(worldZ) -
			std::cos(veinAngle) * static_cast<double>(veinSize) / 8.0;

		double startY = static_cast<double>(blockY) +
			static_cast<double>(random() % 3) - 1.0;
		double endY = static_cast<double>(blockY) +
			static_cast<double>(random() % 3) - 1.0;

		for (int step = 0; step < veinSize; step++) {
			double progress = static_cast<double>(step) / static_cast<double>(veinSize);

			// Interpolate position along the vein line
			double centerX = startX + (endX - startX) * progress;
			double centerY = startY + (endY - startY) * progress;
			double centerZ = startZ + (endZ - startZ) * progress;

			// Radius varies sinusoidally for natural shape
			double sinProgress = std::sin(progress * M_PI);
			double veinRadius = sinProgress * (static_cast<double>(veinSize) / 16.0) + 0.5;

			int radiusCeil = static_cast<int>(std::ceil(veinRadius));

			for (int offsetX = -radiusCeil; offsetX <= radiusCeil; offsetX++) {
				for (int offsetY = -radiusCeil; offsetY <= radiusCeil; offsetY++) {
					for (int offsetZ = -radiusCeil; offsetZ <= radiusCeil; offsetZ++) {
						double distanceSq =
							static_cast<double>(offsetX * offsetX) +
							static_cast<double>(offsetY * offsetY) +
							static_cast<double>(offsetZ * offsetZ);

						if (distanceSq > veinRadius * veinRadius) continue;

						int placeX = static_cast<int>(centerX) + offsetX - chunkX * 16;
						int placeY = static_cast<int>(centerY) + offsetY;
						int placeZ = static_cast<int>(centerZ) + offsetZ - chunkZ * 16;

						if (placeX < 0 || placeX >= 16) continue;
						if (placeY < 1 || placeY >= 255) continue;
						if (placeZ < 0 || placeZ >= 16) continue;

						int index = BlockIndex(placeX, placeY, placeZ);
						if (blocks[index] == BlockState::Stone) {
							blocks[index] = oreBlock;
						}
					}
				}
			}
		}
	}

	void FeatureDecorator::PlaceTrees(uint64_t seed, int chunkX, int chunkZ,
		std::vector<int32_t>& blocks,
		const std::vector<int32_t>& biomeGrid) const {

		std::mt19937_64 random(seed);
		(void)chunkX;
		(void)chunkZ;

		for (int localZ = 2; localZ < 14; localZ++) {
			for (int localX = 2; localX < 14; localX++) {
				int biomeIndex = localZ * 16 + localX;
				int32_t biome = biomeGrid[biomeIndex];

				// Determine tree count based on biome
				int treeCount = 0;
				if (biome == BiomeRegistry::FOREST) {
					treeCount = 10;
				} else if (biome == BiomeRegistry::JUNGLE) {
					treeCount = 50;
				} else if (biome == BiomeRegistry::TAIGA) {
					treeCount = 10;
				} else if (biome == BiomeRegistry::PLAINS) {
					treeCount = 1;
				} else if (biome == BiomeRegistry::SWAMP) {
					treeCount = 4;
				} else if (biome == BiomeRegistry::SAVANNA) {
					treeCount = 2;
				}

				if (treeCount <= 0) continue;

				// Per-column random check against density
				// Tree counts are per chunk; we distribute across the 12x12 inner area
				double treeProbability = static_cast<double>(treeCount) / 144.0;
				double sample = static_cast<double>(random() % 10000) / 10000.0;
				if (sample > treeProbability) continue;

				int surfaceY = GetTopBlock(blocks, localX, localZ);
				if (surfaceY < 1 || surfaceY > 250) continue;

				// Only place on grass or dirt
				int32_t surfaceBlock = blocks[BlockIndex(localX, surfaceY, localZ)];
				if (surfaceBlock != BlockState::GrassBlock &&
					surfaceBlock != BlockState::Dirt &&
					surfaceBlock != BlockState::Podzol) continue;

				// Check the block above is air
				if (surfaceY + 1 < 256 &&
					blocks[BlockIndex(localX, surfaceY + 1, localZ)] != BlockState::Air) continue;

				PlaceTree(blocks, localX, surfaceY + 1, localZ, random());
			}
		}
	}

	void FeatureDecorator::PlaceTree(std::vector<int32_t>& blocks,
		int baseX, int baseY, int baseZ, uint64_t seed) const {

		std::mt19937_64 random(seed);

		int trunkHeight = 4 + static_cast<int>(random() % 3);

		// Check space for the tree
		if (baseY + trunkHeight + 2 >= 256) return;

		// Place trunk
		for (int heightOffset = 0; heightOffset < trunkHeight; heightOffset++) {
			int trunkIndex = BlockIndex(baseX, baseY + heightOffset, baseZ);
			blocks[trunkIndex] = BlockState::OakLog;
		}

		// Place leaf canopy (3 layers)
		int leafStartHeight = trunkHeight - 3;
		for (int heightOffset = leafStartHeight; heightOffset <= trunkHeight + 1; heightOffset++) {
			int leafRadius;
			if (heightOffset == trunkHeight + 1) {
				leafRadius = 0;
			} else if (heightOffset >= trunkHeight) {
				leafRadius = 1;
			} else {
				leafRadius = 2;
			}

			for (int offsetX = -leafRadius; offsetX <= leafRadius; offsetX++) {
				for (int offsetZ = -leafRadius; offsetZ <= leafRadius; offsetZ++) {
					// Skip trunk position below canopy top
					if (offsetX == 0 && offsetZ == 0 && heightOffset < trunkHeight) continue;
					// Skip corners for rounder shape
					if (std::abs(offsetX) == leafRadius && std::abs(offsetZ) == leafRadius) continue;

					int leafX = baseX + offsetX;
					int leafY = baseY + heightOffset;
					int leafZ = baseZ + offsetZ;

					if (leafX < 0 || leafX >= 16) continue;
					if (leafZ < 0 || leafZ >= 16) continue;
					if (leafY < 0 || leafY >= 256) continue;

					int leafIndex = BlockIndex(leafX, leafY, leafZ);
					if (blocks[leafIndex] == BlockState::Air) {
						blocks[leafIndex] = BlockState::OakLeaves;
					}
				}
			}
		}

		// Top leaf block
		if (baseY + trunkHeight < 256) {
			int topIndex = BlockIndex(baseX, baseY + trunkHeight, baseZ);
			if (blocks[topIndex] == BlockState::Air) {
				blocks[topIndex] = BlockState::OakLeaves;
			}
		}
	}

	void FeatureDecorator::PlaceVegetation(uint64_t seed, int chunkX, int chunkZ,
		std::vector<int32_t>& blocks,
		const std::vector<int32_t>& biomeGrid) const {

		std::mt19937_64 random(seed);
		(void)chunkX;
		(void)chunkZ;

		for (int localZ = 0; localZ < 16; localZ++) {
			for (int localX = 0; localX < 16; localX++) {
				int biomeIndex = localZ * 16 + localX;
				int32_t biome = biomeGrid[biomeIndex];

				int surfaceY = GetTopBlock(blocks, localX, localZ);
				if (surfaceY < 1 || surfaceY >= 255) continue;

				int32_t surfaceBlock = blocks[BlockIndex(localX, surfaceY, localZ)];
				int aboveIndex = BlockIndex(localX, surfaceY + 1, localZ);

				// Only place vegetation on air above surface
				if (blocks[aboveIndex] != BlockState::Air) continue;

				double vegetationRoll = static_cast<double>(random() % 10000) / 10000.0;

				if (biome == BiomeRegistry::PLAINS) {
					// Tall grass patches (30% coverage)
					if (surfaceBlock != BlockState::GrassBlock) continue;
					if (vegetationRoll < 0.20) {
						blocks[aboveIndex] = BlockState::ShortGrass;
					} else if (vegetationRoll < 0.22) {
						blocks[aboveIndex] = BlockState::Dandelion;
					} else if (vegetationRoll < 0.24) {
						blocks[aboveIndex] = BlockState::Poppy;
					}
				} else if (biome == BiomeRegistry::FOREST) {
					// Flowers and tall grass
					if (surfaceBlock != BlockState::GrassBlock &&
						surfaceBlock != BlockState::CoarseDirt) continue;
					if (surfaceBlock == BlockState::CoarseDirt) continue;
					if (vegetationRoll < 0.25) {
						blocks[aboveIndex] = BlockState::ShortGrass;
					} else if (vegetationRoll < 0.27) {
						blocks[aboveIndex] = BlockState::Dandelion;
					} else if (vegetationRoll < 0.29) {
						blocks[aboveIndex] = BlockState::Poppy;
					} else if (vegetationRoll < 0.30) {
						blocks[aboveIndex] = BlockState::Fern;
					}
				} else if (biome == BiomeRegistry::DESERT) {
					// Cacti on sand (rare)
					if (surfaceBlock != BlockState::Sand) continue;
					if (vegetationRoll < 0.005) {
						// Place cactus (1-3 blocks tall)
						int cactusHeight = 1 + static_cast<int>(random() % 3);
						for (int height = 1; height <= cactusHeight; height++) {
							int cactusY = surfaceY + height;
							if (cactusY >= 256) break;
							blocks[BlockIndex(localX, cactusY, localZ)] = BlockState::Cactus;
						}
					}
				} else if (biome == BiomeRegistry::SWAMP) {
					// Tall grass, ferns
					if (surfaceBlock != BlockState::GrassBlock &&
						surfaceBlock != BlockState::Mud) continue;
					if (surfaceBlock == BlockState::Mud) continue;
					if (vegetationRoll < 0.20) {
						blocks[aboveIndex] = BlockState::ShortGrass;
					} else if (vegetationRoll < 0.22) {
						blocks[aboveIndex] = BlockState::Fern;
					}
				} else if (biome == BiomeRegistry::TAIGA) {
					// Ferns
					if (surfaceBlock != BlockState::Podzol &&
						surfaceBlock != BlockState::GrassBlock) continue;
					if (vegetationRoll < 0.15) {
						blocks[aboveIndex] = BlockState::Fern;
					} else if (vegetationRoll < 0.20) {
						blocks[aboveIndex] = BlockState::ShortGrass;
					}
				} else if (biome == BiomeRegistry::JUNGLE) {
					// Dense vegetation
					if (surfaceBlock != BlockState::GrassBlock) continue;
					if (vegetationRoll < 0.35) {
						blocks[aboveIndex] = BlockState::ShortGrass;
					} else if (vegetationRoll < 0.40) {
						blocks[aboveIndex] = BlockState::Fern;
					}
				} else if (biome == BiomeRegistry::SAVANNA) {
					// Sparse tall grass
					if (surfaceBlock != BlockState::GrassBlock) continue;
					if (vegetationRoll < 0.10) {
						blocks[aboveIndex] = BlockState::ShortGrass;
					}
				}
			}
		}
	}

	int FeatureDecorator::GetTopBlock(const std::vector<int32_t>& blocks,
		int localX, int localZ) const {

		for (int blockY = 255; blockY >= 0; blockY--) {
			int32_t block = blocks[BlockIndex(localX, blockY, localZ)];
			if (block != BlockState::Air && block != BlockState::Water) {
				return blockY;
			}
		}
		return -1;
	}

}
