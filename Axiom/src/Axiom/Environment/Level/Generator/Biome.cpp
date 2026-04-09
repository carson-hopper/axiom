#include "axpch.h"
#include "Axiom/Environment/Level/Generator/Biome.h"

#include "Axiom/Environment/Level/Generator/BlockStates.h"

namespace Axiom {

	std::vector<BiomeData> BiomeRegistry::s_Biomes;
	bool BiomeRegistry::s_Bootstrapped = false;

	void BiomeRegistry::Bootstrap() {
		if (s_Bootstrapped) return;

		/**
		 * Pre-allocate enough slots so biome IDs can be
		 * used as direct indices into the registry vector.
		 */
		s_Biomes.resize(38);

		s_Biomes[OCEAN] = {
			OCEAN, "ocean",
			BlockState::Gravel, BlockState::Dirt,
			-1.0f, 0.1f,
			0.5f, 0.5f
		};

		s_Biomes[PLAINS] = {
			PLAINS, "plains",
			BlockState::GrassBlock, BlockState::Dirt,
			0.125f, 0.05f,
			0.8f, 0.4f
		};

		s_Biomes[DESERT] = {
			DESERT, "desert",
			BlockState::Sand, BlockState::Sand,
			0.125f, 0.05f,
			2.0f, 0.0f
		};

		s_Biomes[MOUNTAINS] = {
			MOUNTAINS, "mountains",
			BlockState::GrassBlock, BlockState::Dirt,
			1.0f, 0.5f,
			0.2f, 0.3f
		};

		s_Biomes[FOREST] = {
			FOREST, "forest",
			BlockState::GrassBlock, BlockState::Dirt,
			0.1f, 0.2f,
			0.7f, 0.8f
		};

		s_Biomes[TAIGA] = {
			TAIGA, "taiga",
			BlockState::GrassBlock, BlockState::Dirt,
			0.2f, 0.2f,
			0.25f, 0.8f
		};

		s_Biomes[SWAMP] = {
			SWAMP, "swamp",
			BlockState::GrassBlock, BlockState::Dirt,
			-0.2f, 0.1f,
			0.8f, 0.9f
		};

		s_Biomes[RIVER] = {
			RIVER, "river",
			BlockState::Sand, BlockState::Dirt,
			-0.5f, 0.0f,
			0.5f, 0.5f
		};

		/**
		 * Biomes 8 and 9 are unused slots in the classic layout.
		 * We skip them and fill slots 10-37 for the remaining
		 * biome types that have non-contiguous IDs.
		 */

		s_Biomes[FROZEN_OCEAN] = {
			FROZEN_OCEAN, "frozen_ocean",
			BlockState::Gravel, BlockState::Dirt,
			-1.0f, 0.1f,
			0.0f, 0.5f
		};

		s_Biomes[ICE_PLAINS] = {
			ICE_PLAINS, "ice_plains",
			BlockState::GrassBlock, BlockState::Dirt,
			0.125f, 0.05f,
			0.0f, 0.5f
		};

		s_Biomes[MUSHROOM_ISLAND] = {
			MUSHROOM_ISLAND, "mushroom_island",
			BlockState::GrassBlock, BlockState::Dirt,
			0.2f, 0.3f,
			0.9f, 1.0f
		};

		s_Biomes[BEACH] = {
			BEACH, "beach",
			BlockState::Sand, BlockState::Sand,
			0.0f, 0.025f,
			0.8f, 0.4f
		};

		s_Biomes[JUNGLE] = {
			JUNGLE, "jungle",
			BlockState::GrassBlock, BlockState::Dirt,
			0.1f, 0.2f,
			0.95f, 0.9f
		};

		s_Biomes[SAVANNA] = {
			SAVANNA, "savanna",
			BlockState::GrassBlock, BlockState::Dirt,
			0.125f, 0.05f,
			1.2f, 0.0f
		};

		s_Biomes[BADLANDS] = {
			BADLANDS, "badlands",
			BlockState::Terracotta, BlockState::Terracotta,
			0.1f, 0.2f,
			2.0f, 0.0f
		};

		s_Bootstrapped = true;
	}

	const BiomeData* BiomeRegistry::Get(int32_t id) {
		if (id < 0 || id >= static_cast<int32_t>(s_Biomes.size())) {
			return nullptr;
		}

		const BiomeData& biome = s_Biomes[id];
		if (biome.Name.empty()) {
			return nullptr;
		}

		return &biome;
	}

	const BiomeData* BiomeRegistry::GetByName(const std::string& name) {
		for (const BiomeData& biome : s_Biomes) {
			if (biome.Name == name) {
				return &biome;
			}
		}
		return nullptr;
	}

	int32_t BiomeRegistry::Count() {
		int32_t count = 0;
		for (const BiomeData& biome : s_Biomes) {
			if (!biome.Name.empty()) {
				count++;
			}
		}
		return count;
	}

}
