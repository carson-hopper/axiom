#pragma once

#include "Axiom/Environment/World/Generator/Noise.h"

#include <cstdint>

namespace Axiom {

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

	enum class BiomeType {
		Ocean, Beach, Plains, Forest, BirchForest, DarkForest,
		Taiga, SnowyPlains, Desert, Savanna, Swamp, Jungle,
		Badlands, Meadow, Mountain
	};

	/**
	 * Selects biomes based on temperature, humidity, and continentalness
	 * noise. Provides biome-to-registry-ID mapping.
	 */
	class BiomeProvider {
	public:
		explicit BiomeProvider(const uint64_t seed)
			: m_TemperatureNoise(seed)
			, m_HumidityNoise(seed + 1) {}

		BiomeType SelectBiome(const double continentalness, const double erosion,
			const int worldX, const int worldZ) const {

			const double blockX = static_cast<double>(worldX);
			const double blockZ = static_cast<double>(worldZ);

			const double temperature = m_TemperatureNoise.OctaveNoise(blockX * 0.0015, blockZ * 0.0015, 3, 0.5);
			const double humidity = m_HumidityNoise.OctaveNoise(blockX * 0.0015, blockZ * 0.0015, 3, 0.5);

			if (continentalness < -0.3) return BiomeType::Ocean;
			if (continentalness < -0.15) return BiomeType::Beach;

			if (continentalness > 0.6 && erosion < -0.2) return BiomeType::Mountain;

			if (temperature > 0.4) {
				if (humidity > 0.3) return BiomeType::Jungle;
				if (humidity > 0.0) return BiomeType::Savanna;
				if (humidity < -0.3) return BiomeType::Badlands;
				return BiomeType::Desert;
			}

			if (temperature < -0.4) {
				if (humidity > 0.0) return BiomeType::Taiga;
				return BiomeType::SnowyPlains;
			}

			if (humidity > 0.3) {
				if (temperature > 0.1) return BiomeType::DarkForest;
				return BiomeType::Swamp;
			}
			if (humidity > 0.0) return BiomeType::Forest;
			if (humidity > -0.2) return BiomeType::BirchForest;
			if (erosion > 0.2) return BiomeType::Meadow;

			return BiomeType::Plains;
		}

		static int32_t ToRegistryId(const BiomeType biome) {
			switch (biome) {
				case BiomeType::Ocean:       return BiomeId::Ocean;
				case BiomeType::Beach:       return BiomeId::Beach;
				case BiomeType::Plains:      return BiomeId::Plains;
				case BiomeType::Forest:      return BiomeId::Forest;
				case BiomeType::BirchForest: return BiomeId::BirchForest;
				case BiomeType::DarkForest:  return BiomeId::DarkForest;
				case BiomeType::Taiga:       return BiomeId::Taiga;
				case BiomeType::SnowyPlains: return BiomeId::SnowyPlains;
				case BiomeType::Desert:      return BiomeId::Desert;
				case BiomeType::Savanna:     return BiomeId::Savanna;
				case BiomeType::Swamp:       return BiomeId::Swamp;
				case BiomeType::Jungle:      return BiomeId::Jungle;
				case BiomeType::Badlands:    return BiomeId::Badlands;
				case BiomeType::Meadow:      return BiomeId::Meadow;
				case BiomeType::Mountain:    return BiomeId::JaggedPeaks;
				default:                     return BiomeId::Plains;
			}
		}

	private:
		PerlinNoise m_TemperatureNoise;
		PerlinNoise m_HumidityNoise;
	};

}
