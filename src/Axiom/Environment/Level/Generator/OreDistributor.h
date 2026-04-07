#pragma once

#include "Axiom/Environment/Level/Generator/BlockStates.h"
#include "Axiom/Environment/Level/Generator/Noise.h"

#include <cstdint>

namespace Axiom {

	/**
	 * Places ore veins underground by sampling 3D noise at
	 * different frequencies and checking depth-based thresholds.
	 */
	class OreDistributor {
	public:
		explicit OreDistributor(const uint64_t seed)
			: m_OreNoise(seed) {}

		/**
		 * Returns an ore block state ID if this position should
		 * contain an ore, or 0 if no ore should be placed.
		 */
		int32_t GetOre(const int worldX, const int worldY, const int worldZ,
			const bool isDeepslateLayer) const {

			const double blockX = static_cast<double>(worldX);
			const double blockY = static_cast<double>(worldY);
			const double blockZ = static_cast<double>(worldZ);

			const double primarySample = m_OreNoise.Noise3D(
				blockX * 0.1, blockY * 0.1, blockZ * 0.1);
			const double secondarySample = m_OreNoise.Noise3D(
				blockX * 0.15 + 1000, blockY * 0.15, blockZ * 0.15 + 1000);
			const double tertiarySample = m_OreNoise.Noise3D(
				blockX * 0.12 + 2000, blockY * 0.12, blockZ * 0.12 + 2000);

			// Coal: y 0-128, common
			if (worldY > 0 && worldY < 128 && primarySample > 0.75) {
				return isDeepslateLayer ? BlockState::DeepslateCoalOre : BlockState::CoalOre;
			}

			// Iron: y -64 to 72, fairly common
			if (worldY < 72 && secondarySample > 0.78) {
				return isDeepslateLayer ? BlockState::DeepslateIronOre : BlockState::IronOre;
			}

			// Copper: y -16 to 112
			if (worldY > -16 && worldY < 112 && tertiarySample > 0.80) {
				return isDeepslateLayer ? BlockState::DeepslateCopperOre : BlockState::CopperOre;
			}

			// Gold: y -64 to 32, uncommon
			if (worldY < 32 && primarySample > 0.85) {
				return isDeepslateLayer ? BlockState::DeepslateGoldOre : BlockState::GoldOre;
			}

			// Lapis: y -64 to 30
			if (worldY < 30 && secondarySample > 0.87) {
				return isDeepslateLayer ? BlockState::DeepslateLapisOre : BlockState::LapisOre;
			}

			// Redstone: y -64 to 15
			if (worldY < 15 && tertiarySample > 0.84) {
				return isDeepslateLayer ? BlockState::DeepslateRedstoneOre : BlockState::RedstoneOre;
			}

			// Diamond: y -64 to 16, rare
			if (worldY < 16 && primarySample > 0.92) {
				return isDeepslateLayer ? BlockState::DeepslateDiamondOre : BlockState::DiamondOre;
			}

			return 0;
		}

	private:
		PerlinNoise m_OreNoise;
	};

}
