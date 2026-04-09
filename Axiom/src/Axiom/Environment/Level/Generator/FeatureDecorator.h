#pragma once

#include <cstdint>
#include <vector>

namespace Axiom {

	/**
	 * Feature placement for ores, trees, and vegetation in classic
	 * world generation. Decorates a chunk after terrain has been
	 * generated and caves have been carved.
	 */
	class FeatureDecorator {
	public:
		/**
		 * Decorate a chunk with features after terrain generation.
		 * blocks[y * 256 + z * 16 + x] = block state ID
		 * biomeGrid[z * 16 + x] = biome ID per column
		 */
		void Decorate(int64_t worldSeed, int chunkX, int chunkZ,
			std::vector<int32_t>& blocks,
			const std::vector<int32_t>& biomeGrid) const;

	private:
		void PlaceOres(uint64_t seed, int chunkX, int chunkZ,
			std::vector<int32_t>& blocks) const;

		void PlaceTrees(uint64_t seed, int chunkX, int chunkZ,
			std::vector<int32_t>& blocks,
			const std::vector<int32_t>& biomeGrid) const;

		void PlaceVegetation(uint64_t seed, int chunkX, int chunkZ,
			std::vector<int32_t>& blocks,
			const std::vector<int32_t>& biomeGrid) const;

		void PlaceOreVein(std::vector<int32_t>& blocks, uint64_t seed,
			int32_t oreBlock, int veinSize, int chunkX, int chunkZ,
			int minY, int maxY) const;

		void PlaceTree(std::vector<int32_t>& blocks,
			int baseX, int baseY, int baseZ, uint64_t seed) const;

		int GetTopBlock(const std::vector<int32_t>& blocks,
			int localX, int localZ) const;

		static int BlockIndex(int x, int y, int z) {
			return y * 256 + z * 16 + x;
		}
	};

}
