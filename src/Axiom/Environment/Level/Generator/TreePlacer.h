#pragma once

#include "Axiom/Environment/Level/Generator/BiomeProvider.h"
#include "Axiom/Environment/Level/Generator/BlockStates.h"
#include "Axiom/Environment/Level/Generator/Noise.h"

#include <array>
#include <vector>

namespace Axiom {

	/**
	 * Places trees in generated terrain based on biome type
	 * and deterministic noise-based placement.
	 */
	class TreePlacer {
	public:
		static constexpr int MinY = -64;
		static constexpr int MaxY = 319;

		explicit TreePlacer(const uint64_t seed)
			: m_PlacementNoise(seed) {}

		void PlaceTrees(const int32_t chunkX, const int32_t chunkZ,
			const std::array<int, 256>& heightmap,
			const std::array<BiomeType, 256>& biomeMap,
			std::vector<int32_t>& columnBlocks) const {

			// Stay 2 blocks from chunk edges to avoid cross-chunk leaf issues
			for (int localZ = 2; localZ < 14; localZ++) {
				for (int localX = 2; localX < 14; localX++) {
					const int columnIndex = localZ * 16 + localX;
					const int worldX = chunkX * 16 + localX;
					const int worldZ = chunkZ * 16 + localZ;
					const int surfaceHeight = heightmap[columnIndex];
					const BiomeType biome = biomeMap[columnIndex];

					if (surfaceHeight <= 62) continue;

					const double density = GetTreeDensity(biome);
					if (density <= 0.0) continue;

					const double placementSample = m_PlacementNoise.Noise(
						worldX * 0.8, worldZ * 0.8);
					if (placementSample < 1.0 - density) continue;

					const int32_t surfaceBlock = GetBlock(columnBlocks, localX, surfaceHeight, localZ);
					if (surfaceBlock != BlockState::GrassBlock &&
						surfaceBlock != BlockState::Podzol &&
						surfaceBlock != BlockState::Dirt) continue;

					int32_t logBlock, leafBlock;
					int trunkHeight;
					SelectTreeType(biome, placementSample, logBlock, leafBlock, trunkHeight);

					PlaceSingleTree(columnBlocks, localX, surfaceHeight + 1, localZ,
						logBlock, leafBlock, trunkHeight);
				}
			}
		}

	private:
		static double GetTreeDensity(const BiomeType biome) {
			switch (biome) {
				case BiomeType::Forest:
				case BiomeType::BirchForest:
				case BiomeType::DarkForest:  return 0.35;
				case BiomeType::Taiga:       return 0.30;
				case BiomeType::Jungle:      return 0.50;
				case BiomeType::Swamp:       return 0.15;
				case BiomeType::Plains:
				case BiomeType::Meadow:      return 0.02;
				case BiomeType::Savanna:     return 0.05;
				default:                     return 0.0;
			}
		}

		static void SelectTreeType(const BiomeType biome, const double noiseSample,
			int32_t& logBlock, int32_t& leafBlock, int& trunkHeight) {

			switch (biome) {
				case BiomeType::BirchForest:
					logBlock = BlockState::BirchLog;
					leafBlock = BlockState::BirchLeaves;
					trunkHeight = 5 + static_cast<int>(noiseSample * 2);
					break;
				case BiomeType::Taiga:
					logBlock = BlockState::SpruceLog;
					leafBlock = BlockState::SpruceLeaves;
					trunkHeight = 6 + static_cast<int>(noiseSample * 3);
					break;
				case BiomeType::Jungle:
					logBlock = BlockState::OakLog;
					leafBlock = BlockState::OakLeaves;
					trunkHeight = 6 + static_cast<int>(noiseSample * 5);
					break;
				default:
					logBlock = BlockState::OakLog;
					leafBlock = BlockState::OakLeaves;
					trunkHeight = 4 + static_cast<int>(noiseSample * 2);
					break;
			}
		}

		void PlaceSingleTree(std::vector<int32_t>& columnBlocks,
			const int localX, const int baseY, const int localZ,
			const int32_t logBlock, const int32_t leafBlock,
			const int trunkHeight) const {

			// Trunk
			for (int heightOffset = 0; heightOffset < trunkHeight; heightOffset++) {
				SetBlock(columnBlocks, localX, baseY + heightOffset, localZ, logBlock);
			}

			// Leaf canopy
			const int leafStartOffset = trunkHeight - 3;
			for (int heightOffset = leafStartOffset; heightOffset <= trunkHeight + 1; heightOffset++) {
				int leafRadius = (heightOffset < trunkHeight) ? 2 : 1;
				if (heightOffset == trunkHeight + 1) leafRadius = 0;

				for (int offsetX = -leafRadius; offsetX <= leafRadius; offsetX++) {
					for (int offsetZ = -leafRadius; offsetZ <= leafRadius; offsetZ++) {
						if (offsetX == 0 && offsetZ == 0 && heightOffset < trunkHeight) continue;
						if (std::abs(offsetX) == leafRadius && std::abs(offsetZ) == leafRadius) continue;

						SetBlockIfAir(columnBlocks, localX + offsetX, baseY + heightOffset,
							localZ + offsetZ, leafBlock);
					}
				}
			}

			// Top leaf
			SetBlockIfAir(columnBlocks, localX, baseY + trunkHeight, localZ, leafBlock);
		}

		static int32_t GetBlock(const std::vector<int32_t>& columnBlocks,
			const int localX, const int worldY, const int localZ) {

			if (localX < 0 || localX >= 16 || localZ < 0 || localZ >= 16) return BlockState::Air;
			if (worldY < MinY || worldY > MaxY) return BlockState::Air;
			const int absoluteY = worldY - MinY;
			return columnBlocks[absoluteY * 256 + localZ * 16 + localX];
		}

		static void SetBlock(std::vector<int32_t>& columnBlocks,
			const int localX, const int worldY, const int localZ, const int32_t blockState) {

			if (localX < 0 || localX >= 16 || localZ < 0 || localZ >= 16) return;
			if (worldY < MinY || worldY > MaxY) return;
			const int absoluteY = worldY - MinY;
			columnBlocks[absoluteY * 256 + localZ * 16 + localX] = blockState;
		}

		static void SetBlockIfAir(std::vector<int32_t>& columnBlocks,
			const int localX, const int worldY, const int localZ, const int32_t blockState) {

			if (localX < 0 || localX >= 16 || localZ < 0 || localZ >= 16) return;
			if (worldY < MinY || worldY > MaxY) return;
			const int absoluteY = worldY - MinY;
			int32_t& existing = columnBlocks[absoluteY * 256 + localZ * 16 + localX];
			if (existing == BlockState::Air) {
				existing = blockState;
			}
		}

		PerlinNoise m_PlacementNoise;
	};

}
