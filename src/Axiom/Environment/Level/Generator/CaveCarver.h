#pragma once

#include "Axiom/Environment/Level/Generator/NormalNoise.h"

#include <algorithm>
#include <cmath>

namespace Axiom {

	/**
	 * Four cave types matching vanilla 26.1's density-function-driven caves:
	 *
	 * 1. Cheese caves — large interconnected chambers
	 * 2. Spaghetti caves — long winding tunnels
	 * 3. Noodle caves — thin winding passages
	 * 4. Cave entrances — openings connecting surface to underground
	 *
	 * Each cave type produces a density contribution that is subtracted
	 * from the terrain density. Where caves > terrain, air forms.
	 */
	class CaveCarver {
	public:
		explicit CaveCarver(const uint64_t seed)
			: m_CheeseNoise(seed, NoiseParameters::CaveCheese())
			, m_EntranceNoise(seed + 100, NoiseParameters::CaveEntrance())
			, m_LayerNoise(seed + 200, NoiseParameters::CaveLayer())
			, m_Spaghetti2DNoise(seed + 300, NoiseParameters::Spaghetti2D())
			, m_SpaghettiRoughnessNoise(seed + 400, NoiseParameters::SpaghettiRoughness())
			, m_NoodleNoise(seed + 500, NoiseParameters::Noodle())
			, m_NoodleThicknessNoise(seed + 600, NoiseParameters::NoodleThickness())
			, m_NoodleRidgeANoise(seed + 700, NoiseParameters::NoodleRidgeA())
			, m_NoodleRidgeBNoise(seed + 800, NoiseParameters::NoodleRidgeB()) {}

		/**
		 * Compute total cave density at a position.
		 * Positive values mean "carve here" (subtract from terrain density).
		 * Returns a value that should be subtracted from terrain density.
		 */
		double SampleCaveDensity(const int worldX, const int worldY, const int worldZ) const {
			const double blockX = static_cast<double>(worldX);
			const double blockY = static_cast<double>(worldY);
			const double blockZ = static_cast<double>(worldZ);

			double totalCaveDensity = 0.0;

			// Cheese caves: large chambers
			totalCaveDensity = std::max(totalCaveDensity, SampleCheeseCave(blockX, blockY, blockZ));

			// Spaghetti caves: long tunnels
			totalCaveDensity = std::max(totalCaveDensity, SampleSpaghettiCave(blockX, blockY, blockZ));

			// Noodle caves: thin passages
			totalCaveDensity = std::max(totalCaveDensity, SampleNoodleCave(blockX, blockY, blockZ));

			// Cave entrances: surface connections
			totalCaveDensity = std::max(totalCaveDensity, SampleCaveEntrance(blockX, blockY, blockZ));

			return totalCaveDensity;
		}

		/**
		 * Simple check: should this position be carved?
		 * Used when terrain density is known.
		 */
		bool ShouldCarve(const int worldX, const int worldY, const int worldZ,
			const double terrainDensity) const {

			if (worldY <= -60) return false;  // Don't carve near bedrock
			const double caveDensity = SampleCaveDensity(worldX, worldY, worldZ);
			return caveDensity > terrainDensity * 0.5;
		}

	private:
		/**
		 * Cheese caves: large interconnected chambers.
		 * Uses 9-octave noise for varied sizes. Caves form where
		 * the noise value exceeds a threshold that varies with Y.
		 */
		double SampleCheeseCave(const double blockX, const double blockY, const double blockZ) const {
			const double cheeseValue = m_CheeseNoise.GetValue(blockX, blockY, blockZ);

			// Layer noise controls which Y levels have caves
			const double layerValue = m_LayerNoise.GetValue(blockX, blockZ);
			const double targetY = layerValue * 40.0;
			const double yDistance = std::abs(blockY - targetY) / 50.0;
			const double yFactor = std::max(0.0, 1.0 - yDistance * yDistance);

			// Cheese caves form where noise exceeds a low threshold
			const double depthFactor = std::clamp((63.0 - blockY) / 40.0, 0.0, 1.0);

			const double caveStrength = (std::abs(cheeseValue) - 0.08) * yFactor * depthFactor;
			return std::max(0.0, caveStrength * 4.0);
		}

		/**
		 * Spaghetti caves: long winding tunnels.
		 */
		double SampleSpaghettiCave(const double blockX, const double blockY, const double blockZ) const {
			const double noise2D = m_Spaghetti2DNoise.GetValue(blockX * 2.0, blockY * 2.0, blockZ * 2.0);

			const double roughness = m_SpaghettiRoughnessNoise.GetValue(blockX, blockY, blockZ);
			const double tunnelWidth = 0.08 + std::abs(roughness) * 0.04;

			const double tunnelStrength = tunnelWidth - std::abs(noise2D);

			const double depthFactor = std::clamp((63.0 - blockY) / 20.0, 0.0, 1.0);
			return std::max(0.0, tunnelStrength * depthFactor * 8.0);
		}

		/**
		 * Noodle caves: thin winding passages.
		 */
		double SampleNoodleCave(const double blockX, const double blockY, const double blockZ) const {
			const double toggleValue = m_NoodleNoise.GetValue(blockX, blockY, blockZ);
			if (toggleValue < -0.3) return 0.0;

			const double thickness = m_NoodleThicknessNoise.GetValue(blockX, blockY, blockZ);
			const double tunnelWidth = 0.04 + std::abs(thickness) * 0.03;

			const double ridgeA = m_NoodleRidgeANoise.GetValue(blockX * 2.5, blockY * 2.5, blockZ * 2.5);
			const double ridgeB = m_NoodleRidgeBNoise.GetValue(blockX * 2.5, blockY * 2.5, blockZ * 2.5);

			const double ridgeDistance = std::sqrt(ridgeA * ridgeA + ridgeB * ridgeB);
			const double tunnelStrength = tunnelWidth - ridgeDistance;

			const double depthFactor = std::clamp((63.0 - blockY) / 30.0, 0.0, 1.0);
			return std::max(0.0, tunnelStrength * depthFactor * 6.0);
		}

		/**
		 * Cave entrances: openings that connect the surface to underground.
		 */
		double SampleCaveEntrance(const double blockX, const double blockY, const double blockZ) const {
			const double entranceValue = m_EntranceNoise.GetValue(blockX, blockY / 1.5, blockZ);

			// Gaussian centered around Y=30 with wide spread
			const double yFactor = std::exp(-std::pow((blockY - 30.0) / 40.0, 2.0));

			const double entranceStrength = (std::abs(entranceValue) - 0.05) * yFactor;
			return std::max(0.0, entranceStrength * 3.0);
		}

		NormalNoise m_CheeseNoise;
		NormalNoise m_EntranceNoise;
		NormalNoise m_LayerNoise;
		NormalNoise m_Spaghetti2DNoise;
		NormalNoise m_SpaghettiRoughnessNoise;
		NormalNoise m_NoodleNoise;
		NormalNoise m_NoodleThicknessNoise;
		NormalNoise m_NoodleRidgeANoise;
		NormalNoise m_NoodleRidgeBNoise;
	};

}
