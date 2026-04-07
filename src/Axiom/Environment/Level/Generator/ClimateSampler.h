#pragma once

#include "Axiom/Environment/Level/Generator/BiomeProvider.h"
#include "Axiom/Environment/Level/Generator/NormalNoise.h"
#include "Axiom/Environment/Level/Generator/SplineFunction.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <vector>

namespace Axiom {

	/**
	 * 6D climate point used for biome selection.
	 * Matches vanilla's Climate.TargetPoint.
	 */
	struct ClimatePoint {
		double temperature;
		double humidity;
		double continentalness;
		double erosion;
		double depth;
		double weirdness;
	};

	/**
	 * A biome entry in the 6D parameter space with min/max ranges.
	 */
	struct BiomeClimateEntry {
		int32_t biomeRegistryId;
		BiomeType biomeType;
		double temperatureMin, temperatureMax;
		double humidityMin, humidityMax;
		double continentalnessMin, continentalnessMax;
		double erosionMin, erosionMax;
		double depthMin, depthMax;
		double weirdnessMin, weirdnessMax;
	};

	/**
	 * Samples climate parameters at any position and selects biomes
	 * using 6D nearest-neighbor lookup, matching vanilla's
	 * MultiNoiseBiomeSource.
	 */
	class ClimateSampler {
	public:
		static constexpr int SeaLevel = 63;

		explicit ClimateSampler(const uint64_t seed)
			: m_TemperatureNoise(seed, NoiseParameters::Temperature())
			, m_VegetationNoise(seed + 100, NoiseParameters::Vegetation())
			, m_ContinentalnessNoise(seed + 200, NoiseParameters::Continentalness())
			, m_ErosionNoise(seed + 300, NoiseParameters::Erosion())
			, m_RidgeNoise(seed + 400, NoiseParameters::Ridge())
			, m_ShiftXNoise(seed + 500, NoiseParameters::ShiftX())
			, m_ShiftZNoise(seed + 600, NoiseParameters::ShiftZ())
			, m_OffsetNoise(seed + 700, NoiseParameters::Offset())
			, m_TerrainOffsetSpline(SplineFunction::BuildTerrainOffset())
			, m_ErosionFactorSpline(SplineFunction::BuildErosionFactor())
			, m_RidgeFactorSpline(SplineFunction::BuildRidgeFactor()) {

			BuildBiomeTable();
		}

		/**
		 * Sample all climate parameters at a position.
		 */
		ClimatePoint SampleClimate(const int worldX, const int worldY, const int worldZ) const {
			const double blockX = static_cast<double>(worldX);
			const double blockZ = static_cast<double>(worldZ);

			// Vanilla shifts coordinates for temperature/vegetation for spatial decorrelation
			const double shiftX = m_ShiftXNoise.GetValue(blockX, 0, blockZ) * 4.0;
			const double shiftZ = m_ShiftZNoise.GetValue(blockX, 0, blockZ) * 4.0;

			const double temperature = m_TemperatureNoise.GetValue(
				(blockX + shiftX) * 0.25, 0, (blockZ + shiftZ) * 0.25);
			const double humidity = m_VegetationNoise.GetValue(
				(blockX + shiftX) * 0.25, 0, (blockZ + shiftZ) * 0.25);

			const double continentalness = m_ContinentalnessNoise.GetValue(blockX, blockZ);
			const double erosion = m_ErosionNoise.GetValue(blockX, blockZ);
			const double rawRidges = m_RidgeNoise.GetValue(blockX, blockZ);

			// Vanilla PV (peaks & valleys) transform
			const double weirdness = PeaksAndValleys(rawRidges);

			// Depth: Y relative to approximate surface
			const double surfaceY = ComputeApproximateSurfaceY(continentalness, erosion, weirdness);
			const double depth = (surfaceY - static_cast<double>(worldY)) / 128.0;

			return ClimatePoint{temperature, humidity, continentalness, erosion, depth, weirdness};
		}

		/**
		 * Select biome at this position using 6D nearest-neighbor.
		 */
		int32_t SelectBiome(const int worldX, const int worldY, const int worldZ) const {
			const ClimatePoint climate = SampleClimate(worldX, worldY, worldZ);
			return FindNearestBiome(climate);
		}

		/**
		 * Select biome type from climate (for surface decoration etc.)
		 */
		BiomeType SelectBiomeType(const int worldX, const int worldZ) const {
			const ClimatePoint climate = SampleClimate(worldX, 64, worldZ);
			const int32_t biomeId = FindNearestBiome(climate);

			for (const auto& entry : m_BiomeTable) {
				if (entry.biomeRegistryId == biomeId) return entry.biomeType;
			}
			return BiomeType::Plains;
		}

		// ----- Terrain shape accessors ----------------------------------

		double GetContinentalness(const int worldX, const int worldZ) const {
			return m_ContinentalnessNoise.GetValue(
				static_cast<double>(worldX), static_cast<double>(worldZ));
		}

		double GetErosion(const int worldX, const int worldZ) const {
			return m_ErosionNoise.GetValue(
				static_cast<double>(worldX), static_cast<double>(worldZ));
		}

		double GetRidges(const int worldX, const int worldZ) const {
			return m_RidgeNoise.GetValue(
				static_cast<double>(worldX), static_cast<double>(worldZ));
		}

		double GetWeirdness(const int worldX, const int worldZ) const {
			return PeaksAndValleys(GetRidges(worldX, worldZ));
		}

		/**
		 * Compute approximate surface Y using splines.
		 * This is the "preliminary surface level" from the noise router.
		 */
		double ComputeApproximateSurfaceY(const double continentalness,
			const double erosion, const double weirdness) const {

			// Base height from continentalness spline
			const double baseOffset = m_TerrainOffsetSpline.Apply(continentalness);

			// Erosion factor reduces terrain amplitude
			const double erosionFactor = m_ErosionFactorSpline.Apply(erosion);

			// Ridge factor adds peaks/valleys
			const double ridgeFactor = m_RidgeFactorSpline.Apply(weirdness);

			// Combine into world Y
			const double normalizedHeight = baseOffset + ridgeFactor * erosionFactor * 0.5;

			// Map from normalized (-0.1 to 0.8) to world Y
			return 64.0 + normalizedHeight * 256.0;
		}

		double ComputeSurfaceY(const int worldX, const int worldZ) const {
			const double continentalness = GetContinentalness(worldX, worldZ);
			const double erosion = GetErosion(worldX, worldZ);
			const double weirdness = GetWeirdness(worldX, worldZ);
			return ComputeApproximateSurfaceY(continentalness, erosion, weirdness);
		}

		const SplineFunction& TerrainOffsetSpline() const { return m_TerrainOffsetSpline; }
		const SplineFunction& ErosionFactorSpline() const { return m_ErosionFactorSpline; }

	private:
		/**
		 * Vanilla PV transform: abs(abs(noise) - 2/3) - 1/3
		 * Creates peaks at extreme values and valleys in between.
		 */
		static double PeaksAndValleys(const double ridgeNoise) {
			return std::abs(std::abs(ridgeNoise) - 2.0 / 3.0) - 1.0 / 3.0;
		}

		int32_t FindNearestBiome(const ClimatePoint& target) const {
			double bestDistance = std::numeric_limits<double>::max();
			int32_t bestBiome = BiomeId::Plains;

			for (const auto& entry : m_BiomeTable) {
				const double distance = ClimateDistance(target, entry);
				if (distance < bestDistance) {
					bestDistance = distance;
					bestBiome = entry.biomeRegistryId;
				}
			}

			return bestBiome;
		}

		static double ClimateDistance(const ClimatePoint& point, const BiomeClimateEntry& entry) {
			const double temperatureDistance = ParameterDistance(point.temperature, entry.temperatureMin, entry.temperatureMax);
			const double humidityDistance = ParameterDistance(point.humidity, entry.humidityMin, entry.humidityMax);
			const double continentalnessDistance = ParameterDistance(point.continentalness, entry.continentalnessMin, entry.continentalnessMax);
			const double erosionDistance = ParameterDistance(point.erosion, entry.erosionMin, entry.erosionMax);
			const double depthDistance = ParameterDistance(point.depth, entry.depthMin, entry.depthMax);
			const double weirdnessDistance = ParameterDistance(point.weirdness, entry.weirdnessMin, entry.weirdnessMax);

			return temperatureDistance * temperatureDistance
				+ humidityDistance * humidityDistance
				+ continentalnessDistance * continentalnessDistance
				+ erosionDistance * erosionDistance
				+ depthDistance * depthDistance
				+ weirdnessDistance * weirdnessDistance;
		}

		static double ParameterDistance(const double value, const double rangeMin, const double rangeMax) {
			if (value < rangeMin) return rangeMin - value;
			if (value > rangeMax) return value - rangeMax;
			return 0.0;
		}

		/**
		 * Build the biome lookup table with 6D parameter ranges.
		 * Simplified from vanilla's full 60+ entry table.
		 */
		void BuildBiomeTable() {
			// Format: biomeId, biomeType, temp[min,max], humid[min,max], cont[min,max], eros[min,max], depth[min,max], weird[min,max]
			m_BiomeTable = {
				// Ocean biomes (low continentalness)
				{BiomeId::DeepOcean, BiomeType::Ocean, -1.0, 1.0, -1.0, 1.0, -1.2, -0.5, -1.0, 1.0, 0.0, 1.0, -1.0, 1.0},
				{BiomeId::Ocean, BiomeType::Ocean, -1.0, 1.0, -1.0, 1.0, -0.5, -0.2, -1.0, 1.0, 0.0, 1.0, -1.0, 1.0},
				{BiomeId::Beach, BiomeType::Beach, -0.5, 1.0, -1.0, 1.0, -0.2, -0.05, -1.0, 1.0, 0.0, 1.0, -1.0, 1.0},
				{BiomeId::FrozenOcean, BiomeType::SnowyPlains, -1.0, -0.5, -1.0, 1.0, -1.2, -0.2, -1.0, 1.0, 0.0, 1.0, -1.0, 1.0},

				// Cold biomes (low temperature)
				{BiomeId::SnowyPlains, BiomeType::SnowyPlains, -1.0, -0.45, -1.0, 0.0, -0.05, 1.0, -1.0, 1.0, 0.0, 1.0, -1.0, 0.0},
				{BiomeId::Taiga, BiomeType::Taiga, -1.0, -0.15, 0.0, 1.0, -0.05, 0.5, -1.0, 0.5, 0.0, 1.0, -1.0, 0.2},

				// Temperate biomes
				{BiomeId::Plains, BiomeType::Plains, -0.45, 0.2, -1.0, -0.1, -0.05, 0.5, -0.5, 1.0, 0.0, 1.0, -1.0, 0.0},
				{BiomeId::Meadow, BiomeType::Meadow, -0.15, 0.2, -0.3, 0.3, 0.2, 0.6, -1.0, -0.3, 0.0, 1.0, -0.3, 0.3},
				{BiomeId::Forest, BiomeType::Forest, -0.3, 0.3, 0.1, 0.6, -0.05, 0.5, -0.5, 0.5, 0.0, 1.0, -0.4, 0.4},
				{BiomeId::BirchForest, BiomeType::BirchForest, -0.15, 0.2, 0.0, 0.4, -0.05, 0.4, -0.3, 0.3, 0.0, 1.0, -0.2, 0.2},
				{BiomeId::DarkForest, BiomeType::DarkForest, -0.15, 0.3, 0.3, 1.0, -0.05, 0.5, -0.5, 0.3, 0.0, 1.0, -0.4, 0.4},
				{BiomeId::Swamp, BiomeType::Swamp, -0.15, 0.3, 0.3, 1.0, -0.2, 0.1, 0.3, 1.0, 0.0, 1.0, -1.0, 1.0},

				// Warm biomes (high temperature)
				{BiomeId::Savanna, BiomeType::Savanna, 0.2, 0.6, -1.0, 0.0, -0.05, 0.5, -0.5, 0.5, 0.0, 1.0, -0.3, 0.3},
				{BiomeId::Desert, BiomeType::Desert, 0.5, 1.0, -1.0, -0.1, -0.05, 0.5, -0.5, 1.0, 0.0, 1.0, -1.0, 1.0},
				{BiomeId::Jungle, BiomeType::Jungle, 0.3, 1.0, 0.3, 1.0, -0.05, 0.5, -0.5, 0.5, 0.0, 1.0, -0.3, 0.3},
				{BiomeId::Badlands, BiomeType::Badlands, 0.5, 1.0, -1.0, -0.1, 0.3, 1.0, -1.0, -0.3, 0.0, 1.0, -1.0, 1.0},

				// Mountain biomes (high continentalness + low erosion)
				{BiomeId::JaggedPeaks, BiomeType::Mountain, -1.0, 1.0, -1.0, 1.0, 0.5, 1.2, -1.0, -0.5, 0.0, 1.0, 0.2, 1.0},
			};
		}

		NormalNoise m_TemperatureNoise;
		NormalNoise m_VegetationNoise;
		NormalNoise m_ContinentalnessNoise;
		NormalNoise m_ErosionNoise;
		NormalNoise m_RidgeNoise;
		NormalNoise m_ShiftXNoise;
		NormalNoise m_ShiftZNoise;
		NormalNoise m_OffsetNoise;

		SplineFunction m_TerrainOffsetSpline;
		SplineFunction m_ErosionFactorSpline;
		SplineFunction m_RidgeFactorSpline;

		std::vector<BiomeClimateEntry> m_BiomeTable;
	};

}
