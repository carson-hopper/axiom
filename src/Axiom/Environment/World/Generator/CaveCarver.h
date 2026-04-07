#pragma once

#include "Axiom/Environment/World/Generator/NormalNoise.h"

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
			const double targetY = layerValue * 64.0; // Center caves around this Y
			const double yDistance = std::abs(blockY - targetY) / 40.0;
			const double yFactor = std::max(0.0, 1.0 - yDistance);

			// Cheese caves are strongest in the middle underground
			const double depthFactor = std::clamp((64.0 - blockY) / 64.0, 0.0, 1.0);

			const double density = (std::abs(cheeseValue) - 0.3) * yFactor * depthFactor;
			return std::max(0.0, density * 2.0);
		}

		/**
		 * Spaghetti caves: long winding tunnels.
		 * Uses two 2D noise fields whose intersection creates tunnel shapes.
		 */
		double SampleSpaghettiCave(const double blockX, const double blockY, const double blockZ) const {
			// Two perpendicular noise fields — tunnels form at their intersection
			const double noise2D = m_Spaghetti2DNoise.GetValue(blockX * 2.0, blockY * 2.0, blockZ * 2.0);

			// Roughness adds variation to tunnel width
			const double roughness = m_SpaghettiRoughnessNoise.GetValue(blockX, blockY, blockZ);
			const double tunnelWidth = 0.03 + std::abs(roughness) * 0.02;

			// Tunnel forms where the absolute noise value is near zero
			const double tunnelDensity = tunnelWidth - std::abs(noise2D);

			// Reduce near surface
			const double depthFactor = std::clamp((50.0 - blockY) / 30.0, 0.0, 1.0);

			return std::max(0.0, tunnelDensity * depthFactor * 3.0);
		}

		/**
		 * Noodle caves: thin winding passages.
		 */
		double SampleNoodleCave(const double blockX, const double blockY, const double blockZ) const {
			// Toggle noise determines if noodles exist here
			const double toggleValue = m_NoodleNoise.GetValue(blockX, blockY, blockZ);
			if (toggleValue < -0.2) return 0.0;

			// Thickness determines tunnel radius
			const double thickness = m_NoodleThicknessNoise.GetValue(blockX, blockY, blockZ);
			const double tunnelWidth = 0.015 + thickness * 0.01;

			// Two ridge noises create the tunnel shape via intersection
			const double ridgeA = m_NoodleRidgeANoise.GetValue(blockX * 2.5, blockY * 2.5, blockZ * 2.5);
			const double ridgeB = m_NoodleRidgeBNoise.GetValue(blockX * 2.5, blockY * 2.5, blockZ * 2.5);

			const double ridgeDensity = std::sqrt(ridgeA * ridgeA + ridgeB * ridgeB);
			const double noodleDensity = tunnelWidth - ridgeDensity;

			const double depthFactor = std::clamp((60.0 - blockY) / 40.0, 0.0, 1.0);
			return std::max(0.0, noodleDensity * depthFactor * 4.0);
		}

		/**
		 * Cave entrances: openings that connect the surface to underground.
		 */
		double SampleCaveEntrance(const double blockX, const double blockY, const double blockZ) const {
			const double entranceValue = m_EntranceNoise.GetValue(blockX, blockY / 1.5, blockZ);

			// Entrances are strongest near sea level
			const double yFactor = std::exp(-std::pow((blockY - 20.0) / 30.0, 2.0));

			const double density = (std::abs(entranceValue) - 0.2) * yFactor;
			return std::max(0.0, density * 1.5);
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
