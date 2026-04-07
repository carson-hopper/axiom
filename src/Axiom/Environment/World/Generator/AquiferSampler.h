#pragma once

#include "Axiom/Environment/World/Generator/BlockStates.h"
#include "Axiom/Environment/World/Generator/NormalNoise.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace Axiom {

	/**
	 * Simulates Minecraft's aquifer system: independent underground
	 * fluid pockets with varying levels, separated by stone barriers.
	 *
	 * Uses a 3D grid of sample points. At each point, noise determines:
	 * - Floodedness: whether the pocket is flooded (>1.0 = flooded)
	 * - Spread: local fluid level variation
	 * - Barrier: thin stone walls between adjacent fluid pockets
	 *
	 * Below Y=-54, fluid is lava instead of water.
	 */
	class AquiferSampler {
	public:
		static constexpr int SeaLevel = 63;
		static constexpr int LavaLevel = -54;
		static constexpr int GridResolution = 16;  // Sample every 16 blocks

		explicit AquiferSampler(const uint64_t seed)
			: m_BarrierNoise(seed, NoiseParameters::AquiferBarrier())
			, m_FloodednessNoise(seed + 100, NoiseParameters::AquiferFloodedness())
			, m_SpreadNoise(seed + 200, NoiseParameters::AquiferSpread())
			, m_LavaNoise(seed + 300, NoiseParameters::AquiferLava()) {}

		/**
		 * Determine what fluid (if any) should be at this position.
		 *
		 * @param worldX, worldY, worldZ Block coordinates
		 * @param terrainDensity The terrain density at this point (positive = solid)
		 * @return Block state: Stone (keep terrain), Water, Lava, or Air
		 */
		int32_t ComputeBlockState(const int worldX, const int worldY, const int worldZ,
			const double terrainDensity) const {

			// Above sea level: standard behavior
			if (worldY > SeaLevel) {
				if (terrainDensity > 0) return BlockState::Stone;
				return BlockState::Air;
			}

			// Solid terrain above aquifer threshold
			if (terrainDensity > 0.25) return BlockState::Stone;

			// Sample aquifer at this position
			const double floodedness = m_FloodednessNoise.GetValue(
				static_cast<double>(worldX), static_cast<double>(worldY) * 0.67, static_cast<double>(worldZ));

			// Not flooded — behave normally
			if (floodedness < -0.3) {
				if (terrainDensity > 0) return BlockState::Stone;
				if (worldY <= SeaLevel) return BlockState::Water;
				return BlockState::Air;
			}

			// Check barrier between fluid pockets
			const double barrier = m_BarrierNoise.GetValue(
				static_cast<double>(worldX), static_cast<double>(worldY) * 0.5, static_cast<double>(worldZ));

			// Strong barrier = keep as stone even if terrain density is low
			if (std::abs(barrier) > 0.3 && terrainDensity > -0.1) {
				return BlockState::Stone;
			}

			// Determine fluid level for this pocket
			const double spread = m_SpreadNoise.GetValue(
				static_cast<double>(worldX), static_cast<double>(worldY) * 0.714, static_cast<double>(worldZ));

			const int fluidLevel = SeaLevel + static_cast<int>(spread * 10.0);

			// Below lava level: lava
			if (worldY < LavaLevel) {
				const double lavaValue = m_LavaNoise.GetValue(
					static_cast<double>(worldX), static_cast<double>(worldY), static_cast<double>(worldZ));
				if (lavaValue > 0.0 && terrainDensity <= 0) {
					return BlockState::Lava;
				}
			}

			// Fluid placement
			if (terrainDensity <= 0) {
				if (worldY < fluidLevel) {
					return (worldY < LavaLevel) ? BlockState::Lava : BlockState::Water;
				}
				return BlockState::Air;
			}

			return BlockState::Stone;
		}

	private:
		NormalNoise m_BarrierNoise;
		NormalNoise m_FloodednessNoise;
		NormalNoise m_SpreadNoise;
		NormalNoise m_LavaNoise;
	};

}
