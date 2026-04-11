#pragma once

#include "Axiom/Environment/Level/Generator/ChunkGenerator.h"
#include "Axiom/Environment/Level/Generator/PerlinNoise.h"
#include "Axiom/Environment/Level/Generator/BiomeProvider.h"
#include "Axiom/Environment/Level/Generator/Biome.h"
#include "Axiom/Environment/Level/Generator/CaveGenerator.h"
#include "Axiom/Environment/Level/Generator/FeatureDecorator.h"

#include <cstdint>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace Axiom {

	/**
	 * Classic noise-based terrain generator that ties together
	 * heightmap generation, biome surfaces, cave carving,
	 * and feature decoration into a full world pipeline.
	 */
	class ClassicLevelSource : public ChunkGenerator {
	public:
		explicit ClassicLevelSource(int64_t seed = 0);

		ChunkData Generate(int32_t chunkX, int32_t chunkZ,
			ChunkTier tier = ChunkTier::Full) override;
		ChunkData Decorate(int32_t chunkX, int32_t chunkZ) override;
		double SpawnY() const override;
		int32_t GetBlockAt(int32_t worldX, int32_t worldY, int32_t worldZ) const override;

	private:
		static constexpr int SEA_LEVEL = 63;
		static constexpr int WORLD_HEIGHT = 256;

		/**
		 * Fill stone and water based on
		 * noise heightmap sampling.
		 */
		void PrepareHeights(int chunkX, int chunkZ,
			std::vector<int32_t>& blocks,
			std::vector<int32_t>& biomeGrid) const;

		/**
		 * Apply biome-specific surface
		 * blocks over raw terrain.
		 */
		void BuildSurfaces(int chunkX, int chunkZ,
			std::vector<int32_t>& blocks,
			const std::vector<int32_t>& biomeGrid) const;

		/**
		 * Encode block array into ChunkData
		 * for network transmission.
		 */
		ChunkData EncodeChunk(int chunkX, int chunkZ,
			const std::vector<int32_t>& blocks,
			const std::vector<int32_t>& biomeGrid) const;

		/**
		 * Compute per-section sky light from block data.
		 * Traces downward from the top of each column —
		 * light is 15 above the highest opaque block, 0 below.
		 */
		std::vector<std::vector<uint8_t>> ComputeSkyLight(
			const std::vector<int32_t>& blocks) const;

		/**
		 * Cache blocks for GetBlockAt
		 * physics queries.
		 */
		void CacheBlocks(int chunkX, int chunkZ,
			const std::vector<int32_t>& blocks) const;

		int64_t m_Seed;
		ClassicPerlinNoise m_NoiseMain;
		ClassicPerlinNoise m_NoiseDetail;
		ClassicPerlinNoise m_NoiseSurface;
		BiomeSource m_BiomeSource;
		CaveGenerator m_CaveGenerator;
		FeatureDecorator m_FeatureDecorator;

		mutable std::mutex m_CacheMutex;
		mutable std::unordered_map<int64_t, std::vector<int32_t>> m_BlockCache;

		static int64_t CacheKey(int32_t chunkX, int32_t chunkZ) {
			return (static_cast<int64_t>(chunkX) << 32) | static_cast<uint32_t>(chunkZ);
		}
	};

}
