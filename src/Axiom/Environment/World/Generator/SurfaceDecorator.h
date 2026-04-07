#pragma once

#include "Axiom/Environment/World/Generator/BiomeProvider.h"
#include "Axiom/Environment/World/Generator/BlockStates.h"
#include "Axiom/Environment/World/Generator/Noise.h"

#include <cstdint>

namespace Axiom {

	/**
	 * Determines surface and sub-surface block types based on biome,
	 * and places vegetation (grass, flowers, snow) on the surface.
	 */
	class SurfaceDecorator {
	public:
		static constexpr int SeaLevel = 62;

		explicit SurfaceDecorator(const uint64_t seed)
			: m_VegetationNoise(seed)
			, m_FlowerNoise(seed + 1) {}

		int32_t GetSurfaceBlock(const int surfaceHeight, const BiomeType biome) const {
			if (surfaceHeight < SeaLevel - 2) return BlockState::Gravel;
			if (surfaceHeight <= SeaLevel + 1 && surfaceHeight >= SeaLevel - 2) return BlockState::Sand;

			switch (biome) {
				case BiomeType::Desert:      return BlockState::Sand;
				case BiomeType::Badlands:    return BlockState::Terracotta;
				case BiomeType::Taiga:       return BlockState::Podzol;
				case BiomeType::Swamp:       return BlockState::Mud;
				case BiomeType::Mountain:    return BlockState::Stone;
				default:                     return BlockState::GrassBlock;
			}
		}

		int32_t GetSubSurfaceBlock(const int surfaceHeight, const BiomeType biome) const {
			if (surfaceHeight <= SeaLevel + 1) return BlockState::Sand;

			switch (biome) {
				case BiomeType::Desert:      return BlockState::Sandstone;
				case BiomeType::Badlands:    return BlockState::Terracotta;
				default:                     return BlockState::Dirt;
			}
		}

		/**
		 * Returns a vegetation block to place above the surface, or Air if none.
		 */
		int32_t GetVegetation(const int worldX, const int worldZ, const BiomeType biome) const {
			const double blockX = static_cast<double>(worldX);
			const double blockZ = static_cast<double>(worldZ);

			if (biome == BiomeType::SnowyPlains || biome == BiomeType::Mountain) {
				return BlockState::Snow;
			}

			const double vegetationSample = m_VegetationNoise.Noise(blockX * 0.5 + 500, blockZ * 0.5 + 500);
			const double grassThreshold = 1.0 - GetGrassDensity(biome);

			if (vegetationSample <= grassThreshold) {
				return BlockState::Air;
			}

			const double flowerSample = m_FlowerNoise.Noise(blockX * 1.3, blockZ * 1.3);
			if (flowerSample > 0.85) {
				return (flowerSample > 0.92) ? BlockState::Poppy : BlockState::Dandelion;
			}

			return (biome == BiomeType::Taiga) ? BlockState::Fern : BlockState::ShortGrass;
		}

	private:
		static double GetGrassDensity(const BiomeType biome) {
			switch (biome) {
				case BiomeType::Plains:
				case BiomeType::Meadow:      return 0.4;
				case BiomeType::Forest:
				case BiomeType::BirchForest: return 0.3;
				case BiomeType::Taiga:       return 0.25;
				case BiomeType::Jungle:      return 0.5;
				case BiomeType::Swamp:       return 0.35;
				case BiomeType::Savanna:     return 0.15;
				default:                     return 0.05;
			}
		}

		PerlinNoise m_VegetationNoise;
		PerlinNoise m_FlowerNoise;
	};

}
