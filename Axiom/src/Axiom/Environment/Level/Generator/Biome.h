#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace Axiom {

	/**
	 * Data describing a single biome's terrain characteristics
	 * and surface block composition.
	 */
	struct BiomeData {
		int32_t Id;
		std::string Name;
		int32_t TopBlock;
		int32_t FillerBlock;
		float Depth;
		float Scale;
		float Temperature;
		float Rainfall;
	};

	/**
	 * Registry of all classic biome definitions. Must be bootstrapped
	 * once at startup before any biome lookups are performed.
	 */
	class BiomeRegistry {
	public:
		static void Bootstrap();
		static const BiomeData* Get(int32_t id);
		static const BiomeData* GetByName(const std::string& name);
		static int32_t Count();

		static constexpr int32_t OCEAN = 0;
		static constexpr int32_t PLAINS = 1;
		static constexpr int32_t DESERT = 2;
		static constexpr int32_t MOUNTAINS = 3;
		static constexpr int32_t FOREST = 4;
		static constexpr int32_t TAIGA = 5;
		static constexpr int32_t SWAMP = 6;
		static constexpr int32_t RIVER = 7;
		static constexpr int32_t FROZEN_OCEAN = 10;
		static constexpr int32_t ICE_PLAINS = 12;
		static constexpr int32_t MUSHROOM_ISLAND = 14;
		static constexpr int32_t BEACH = 16;
		static constexpr int32_t JUNGLE = 21;
		static constexpr int32_t SAVANNA = 35;
		static constexpr int32_t BADLANDS = 37;

	private:
		static std::vector<BiomeData> s_Biomes;
		static bool s_Bootstrapped;
	};

}
