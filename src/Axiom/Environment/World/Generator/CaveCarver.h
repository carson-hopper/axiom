#pragma once

#include "Axiom/Environment/World/Generator/Noise.h"

#include <algorithm>

namespace Axiom {

	/**
	 * Carves caves into terrain using intersecting 3D noise fields.
	 * Uses the "swiss cheese" technique: caves form where two
	 * independent noise fields both have low values.
	 */
	class CaveCarver {
	public:
		explicit CaveCarver(const uint64_t seed)
			: m_PrimaryNoise(seed)
			, m_SecondaryNoise(seed + 1) {}

		bool ShouldCarve(const int worldX, const int worldY, const int worldZ,
			const int seaLevel) const {

			const double blockX = static_cast<double>(worldX);
			const double blockY = static_cast<double>(worldY);
			const double blockZ = static_cast<double>(worldZ);

			const double primaryDensity = m_PrimaryNoise.Noise3D(
				blockX * 0.04, blockY * 0.04, blockZ * 0.04);
			const double secondaryDensity = m_SecondaryNoise.Noise3D(
				blockX * 0.04, blockY * 0.04, blockZ * 0.04);

			const double combinedDensity = primaryDensity * primaryDensity
				+ secondaryDensity * secondaryDensity;

			// Caves become rarer near the surface
			const double depthRatio = std::clamp((seaLevel - worldY) / 30.0, 0.0, 1.0);
			const double threshold = 0.02 + (1.0 - depthRatio) * 0.03;

			return combinedDensity < threshold;
		}

	private:
		PerlinNoise m_PrimaryNoise;
		PerlinNoise m_SecondaryNoise;
	};

}
