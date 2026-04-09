#pragma once

#include <cmath>
#include <cstdint>
#include <vector>

namespace Axiom {

	/**
	 * Classic worm-style cave carving following the MCLCE CaveFeature algorithm.
	 * Generates interconnected tunnels by random-walking through chunks,
	 * carving spheroids at each step with varying radius.
	 */
	class CaveGenerator {
	public:
		/**
		 * Carve caves into a 16x256x16 block array.
		 * blocks[y * 256 + z * 16 + x] = block state ID
		 */
		void Generate(int64_t worldSeed, int chunkX, int chunkZ,
			std::vector<int32_t>& blocks) const;

	private:
		static constexpr int SeaLevel = 63;
		static constexpr int LavaLevel = 10;
		static constexpr int ChunkRadius = 4;

		void GenerateChunk(int64_t seed, int originChunkX, int originChunkZ,
			int targetChunkX, int targetChunkZ,
			std::vector<int32_t>& blocks) const;

		void CarveTunnel(int64_t seed, int originChunkX, int originChunkZ,
			int targetChunkX, int targetChunkZ,
			double startX, double startY, double startZ,
			float radius, float yaw, float pitch,
			int startStep, int totalSteps,
			std::vector<int32_t>& blocks) const;

		static bool IsBlockCarveTarget(int32_t stateId);

		static int BlockIndex(int x, int y, int z) {
			return y * 256 + z * 16 + x;
		}
	};

}
