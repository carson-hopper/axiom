#pragma once

#include "Axiom/Environment/Level/Generator/BlockStates.h"
#include "Axiom/Environment/Level/Generator/NormalNoise.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace Axiom {

	/**
	 * Determines fluid placement in terrain.
	 *
	 * Above sea level: air where density <= 0.
	 * Below sea level: water fills only where the terrain naturally
	 * has negative density (oceans, rivers) — NOT in carved caves.
	 * Underground aquifer pockets use floodedness noise to create
	 * rare isolated water/lava pools.
	 */
	class AquiferSampler {
	public:
		static constexpr int SeaLevel = 63;
		static constexpr int LavaLevel = -54;

		explicit AquiferSampler(const uint64_t seed)
			: m_FloodednessNoise(seed + 100, NoiseParameters::AquiferFloodedness())
			, m_LavaNoise(seed + 300, NoiseParameters::AquiferLava()) {}

		/**
		 * Determine block state from terrain density.
		 *
		 * @param terrainDensity Raw density from the density function (before cave carving)
		 * @param finalDensity Density after cave carving
		 */
		int32_t ComputeBlockState(const int worldX, const int worldY, const int worldZ,
			const double terrainDensity, const double finalDensity) const {

			// Solid terrain
			if (finalDensity > 0) {
				if (worldY <= MinY + 5) return BlockState::Bedrock;
				if (worldY <= 0) return BlockState::Deepslate;
				return BlockState::Stone;
			}

			// Above sea level: always air if not solid
			if (worldY > SeaLevel) return BlockState::Air;

			// Terrain was originally solid but got carved by caves
			const bool isCave = terrainDensity > 0 && finalDensity <= 0;

			if (isCave) {
				// Caves above lava level are air, not water
				if (worldY > LavaLevel) return BlockState::Air;

				// Caves below lava level might have lava pools
				const double lavaValue = m_LavaNoise.GetValue(
					static_cast<double>(worldX), static_cast<double>(worldY), static_cast<double>(worldZ));
				if (lavaValue > 0.3) return BlockState::Lava;
				return BlockState::Air;
			}

			// Natural ocean/river: terrain was never solid here
			// Fill with water up to sea level
			if (worldY < LavaLevel) {
				// Deep underground aquifer check
				const double floodedness = m_FloodednessNoise.GetValue(
					static_cast<double>(worldX), static_cast<double>(worldY) * 0.67,
					static_cast<double>(worldZ));
				if (floodedness > 0.5) return BlockState::Lava;
			}

			return BlockState::Water;
		}

		/**
		 * Simplified version when we don't have separate pre/post cave density.
		 */
		int32_t ComputeBlockState(const int worldX, const int worldY, const int worldZ,
			const double density) const {

			return ComputeBlockState(worldX, worldY, worldZ, density, density);
		}

	private:
		static constexpr int MinY = -64;

		NormalNoise m_FloodednessNoise;
		NormalNoise m_LavaNoise;
	};

}
