#pragma once

#include "Axiom/Environment/Level/Generator/Noise.h"

#include <algorithm>
#include <cmath>

namespace Axiom {

	/**
	 * Computes 3D terrain density at any world position.
	 *
	 * Positive density = solid block, negative = air/water.
	 *
	 * The density function combines:
	 * - A base gradient that decreases with height (creates ground level)
	 * - Continental noise that controls land vs ocean scale
	 * - 3D terrain noise that creates overhangs, cliffs, and ridges
	 * - Erosion that smooths or sharpens terrain features
	 * - Squeeze factor near the surface to flatten tops of hills
	 */
	class DensityProvider {
	public:
		static constexpr int SeaLevel = 62;
		static constexpr int MinY = -64;

		explicit DensityProvider(const uint64_t seed)
			: m_ContinentNoise(seed)
			, m_ErosionNoise(seed + 1)
			, m_TerrainNoise(seed + 2)
			, m_RidgeNoise(seed + 3)
			, m_DetailNoise(seed + 4)
			, m_SqueezeNoise(seed + 5)
			, m_Noise3DPrimary(seed + 6)
			, m_Noise3DSecondary(seed + 7) {}

		/**
		 * Sample terrain density at a world position.
		 * Returns positive for solid, negative for air.
		 */
		double SampleDensity(const int worldX, const int worldY, const int worldZ) const {
			const double blockX = static_cast<double>(worldX);
			const double blockY = static_cast<double>(worldY);
			const double blockZ = static_cast<double>(worldZ);

			// Base height gradient: solid below ~64, air above
			// The "target height" varies by continent/terrain noise
			const double targetHeight = ComputeTargetHeight(blockX, blockZ);

			// Vertical gradient: strongly positive below target, strongly negative above
			// Smoothly transitions around the target height
			const double heightDelta = targetHeight - blockY;
			const double baseGradient = heightDelta * 0.1;

			// 3D noise adds overhangs, cliffs, and cave-like features
			const double terrainNoise3D = Sample3DTerrain(blockX, blockY, blockZ);

			// The 3D noise has more influence near the surface
			// Deep underground and high in the air, the gradient dominates
			const double surfaceDistance = std::abs(heightDelta);
			const double noiseInfluence = std::exp(-surfaceDistance * 0.03) * 0.8;

			// Combine
			double density = baseGradient + terrainNoise3D * noiseInfluence;

			// Squeeze: reduce density above target height to flatten hilltops slightly
			// This prevents pure 3D noise from creating too many floating islands
			if (blockY > targetHeight + 10) {
				const double squeezeAmount = (blockY - targetHeight - 10) * 0.02;
				density -= squeezeAmount;
			}

			// Floor: always solid near bedrock
			if (worldY <= MinY + 5) {
				density = 1.0;
			}

			return density;
		}

		/**
		 * Compute the 2D target surface height for a column.
		 * Used for biome selection and surface decoration.
		 */
		int ComputeSurfaceHeight(const int worldX, const int worldZ) const {
			const double targetHeight = ComputeTargetHeight(
				static_cast<double>(worldX), static_cast<double>(worldZ));

			// Walk down from target height to find where density first goes positive
			// (This is approximate — the actual surface from 3D density may differ)
			return std::clamp(static_cast<int>(targetHeight), MinY + 1, 319);
		}

		double GetContinentalness(const int worldX, const int worldZ) const {
			return m_ContinentNoise.OctaveNoise(worldX * 0.0008, worldZ * 0.0008, 4, 0.5);
		}

		double GetErosion(const int worldX, const int worldZ) const {
			return m_ErosionNoise.OctaveNoise(worldX * 0.002, worldZ * 0.002, 3, 0.5);
		}

	private:
		double ComputeTargetHeight(const double blockX, const double blockZ) const {
			const double continentalness = m_ContinentNoise.OctaveNoise(blockX * 0.0008, blockZ * 0.0008, 4, 0.5);
			const double erosion = m_ErosionNoise.OctaveNoise(blockX * 0.002, blockZ * 0.002, 3, 0.5);
			const double terrain = m_TerrainNoise.OctaveNoise(blockX * 0.006, blockZ * 0.006, 5, 0.55);
			const double ridge = m_RidgeNoise.OctaveNoise(blockX * 0.003, blockZ * 0.003, 4, 0.5);
			const double detail = m_DetailNoise.OctaveNoise(blockX * 0.025, blockZ * 0.025, 3, 0.4);

			// Continental base: ocean vs land
			// Spline-like mapping: sharp transition at coastline
			double baseHeight;
			if (continentalness < -0.3) {
				// Deep ocean
				baseHeight = 30.0 + continentalness * 20.0;
			} else if (continentalness < -0.1) {
				// Shallow ocean / coast — steep transition
				const double coastFactor = (continentalness + 0.3) / 0.2;
				baseHeight = 30.0 + coastFactor * 35.0;
			} else if (continentalness < 0.3) {
				// Lowlands
				baseHeight = 65.0 + continentalness * 10.0;
			} else if (continentalness < 0.6) {
				// Highlands
				const double highlandFactor = (continentalness - 0.3) / 0.3;
				baseHeight = 68.0 + highlandFactor * 30.0;
			} else {
				// Mountains
				baseHeight = 98.0 + (continentalness - 0.6) * 60.0;
			}

			// Erosion: smooth peaks or carve valleys
			const double erosionEffect = erosion * 15.0;
			if (erosion > 0) {
				// Positive erosion flattens terrain toward sea level
				baseHeight -= erosionEffect * std::max(0.0, (baseHeight - 64.0) / 40.0);
			} else {
				// Negative erosion creates sharper features
				baseHeight -= erosionEffect * 0.5;
			}

			// Terrain variation
			double terrainEffect = terrain * 12.0;
			// Scale terrain amplitude by base height: more variation on highlands
			terrainEffect *= std::max(0.3, (baseHeight - 50.0) / 40.0);

			// Ridges: creates mountain ridges when continent and ridge align
			if (continentalness > 0.3) {
				const double ridgeStrength = std::abs(ridge);
				const double ridgeHeight = ridgeStrength * 25.0 * std::min(1.0, (continentalness - 0.3) * 3.0);
				baseHeight += ridgeHeight;
			}

			// Fine detail
			const double detailEffect = detail * 3.0;

			return baseHeight + terrainEffect + detailEffect;
		}

		double Sample3DTerrain(const double blockX, const double blockY, const double blockZ) const {
			// Primary 3D noise: large-scale terrain shaping
			const double primary = m_Noise3DPrimary.Noise3D(
				blockX * 0.012, blockY * 0.015, blockZ * 0.012);

			// Secondary 3D noise: finer detail, different scale
			const double secondary = m_Noise3DSecondary.Noise3D(
				blockX * 0.035, blockY * 0.04, blockZ * 0.035);

			// The Y frequency is slightly higher than X/Z to create
			// more horizontal features (overhangs, shelves) than vertical
			return primary * 0.7 + secondary * 0.3;
		}

		PerlinNoise m_ContinentNoise;
		PerlinNoise m_ErosionNoise;
		PerlinNoise m_TerrainNoise;
		PerlinNoise m_RidgeNoise;
		PerlinNoise m_DetailNoise;
		PerlinNoise m_SqueezeNoise;
		PerlinNoise m_Noise3DPrimary;
		PerlinNoise m_Noise3DSecondary;
	};

}
