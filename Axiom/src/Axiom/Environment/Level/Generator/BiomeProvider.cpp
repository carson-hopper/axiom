#include "axpch.h"
#include "Axiom/Environment/Level/Generator/BiomeProvider.h"

namespace Axiom {

	BiomeSource::BiomeSource(int64_t worldSeed) {
		/**
		 * Build the classic layer stack:
		 * IslandLayer -> zooms and island additions -> biome assignment
		 * -> more zooms -> shore detection -> final smooth pass.
		 */
		auto island = Ref<IslandLayer>::Create(1);

		auto zoom1 = Ref<ZoomLayer>::Create(2000, island);
		auto addIsland1 = Ref<AddIslandLayer>::Create(1, zoom1);
		auto zoom2 = Ref<ZoomLayer>::Create(2001, addIsland1);
		auto addIsland2 = Ref<AddIslandLayer>::Create(2, zoom2);

		auto biome = Ref<BiomeLayer>::Create(200, addIsland2);

		auto zoom3 = Ref<ZoomLayer>::Create(1000, biome);
		auto zoom4 = Ref<ZoomLayer>::Create(1001, zoom3);

		auto shore = Ref<ShoreLayer>::Create(100, zoom4);

		auto zoom5 = Ref<ZoomLayer>::Create(1002, shore);
		auto zoom6 = Ref<ZoomLayer>::Create(1003, zoom5);

		auto smooth = Ref<SmoothLayer>::Create(10, zoom6);

		smooth->InitWorldSeed(worldSeed);
		m_BiomeLayer = smooth;
	}

	int32_t BiomeSource::GetBiome(int blockX, int blockZ) const {
		int64_t key = PackCoord(blockX, blockZ);

		{
			std::lock_guard<std::mutex> lock(m_CacheMutex);
			auto iterator = m_Cache.find(key);
			if (iterator != m_Cache.end()) {
				return iterator->second;
			}
		}

		/**
		 * Query the layer stack for a single cell. The layer system
		 * works in biome-scale coordinates (4:1 block ratio).
		 */
		std::vector<int> area = m_BiomeLayer->GetArea(blockX >> 2, blockZ >> 2, 1, 1);
		int32_t biomeId = area[0];

		{
			std::lock_guard<std::mutex> lock(m_CacheMutex);
			m_Cache[key] = biomeId;
		}

		return biomeId;
	}

	std::vector<int32_t> BiomeSource::GetBiomes(int areaX, int areaZ, int width, int height) const {
		std::vector<int> layerResult = m_BiomeLayer->GetArea(areaX, areaZ, width, height);

		std::vector<int32_t> result(layerResult.size());
		for (size_t index = 0; index < layerResult.size(); index++) {
			result[index] = static_cast<int32_t>(layerResult[index]);
		}

		return result;
	}

	int64_t BiomeSource::PackCoord(int x, int z) {
		return (static_cast<int64_t>(x) << 32) | (static_cast<int64_t>(z) & 0xFFFFFFFF);
	}

}
