#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Environment/Level/Generator/Layer.h"
#include "Axiom/Environment/Level/Generator/Layers.h"

#include <cstdint>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace Axiom {

	/**
	 * Modern biome registry IDs used by the terrain chunk
	 * generator and feature decorator for biome lookups.
	 */
	namespace BiomeId {
		constexpr int32_t Plains = 40;
		constexpr int32_t Desert = 14;
		constexpr int32_t Forest = 21;
		constexpr int32_t BirchForest = 4;
		constexpr int32_t DarkForest = 8;
		constexpr int32_t Taiga = 55;
		constexpr int32_t SnowyPlains = 46;
		constexpr int32_t Ocean = 35;
		constexpr int32_t DeepOcean = 13;
		constexpr int32_t Beach = 3;
		constexpr int32_t Swamp = 54;
		constexpr int32_t Jungle = 28;
		constexpr int32_t Savanna = 42;
		constexpr int32_t Badlands = 0;
		constexpr int32_t Meadow = 32;
		constexpr int32_t FrozenOcean = 22;
		constexpr int32_t JaggedPeaks = 27;
	}

	/**
	 * Classic biome source that builds a layer stack to produce
	 * biome IDs for any world coordinate. Results are cached
	 * per-coordinate for thread-safe repeated lookups.
	 */
	class BiomeSource {
	public:
		explicit BiomeSource(int64_t worldSeed);

		int32_t GetBiome(int blockX, int blockZ) const;
		std::vector<int32_t> GetBiomes(int areaX, int areaZ, int width, int height) const;

	private:
		static int64_t PackCoord(int x, int z);

		Ref<Layer> m_BiomeLayer;
		mutable std::mutex m_CacheMutex;
		mutable std::unordered_map<int64_t, int32_t> m_Cache;
	};

}
