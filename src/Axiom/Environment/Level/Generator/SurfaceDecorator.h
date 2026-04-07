#pragma once

#include "Axiom/Environment/Level/Generator/BiomeProvider.h"
#include "Axiom/Environment/Level/Generator/BlockStates.h"
#include "Axiom/Environment/Level/Generator/Noise.h"

#include <cstdint>

namespace Axiom {

	/**
	 * Determines surface and sub-surface block types based on biome
	 * and noise-driven variation. Places vegetation on the surface.
	 *
	 * Surface noise creates natural patches of coarse dirt, gravel,
	 * stone, and other variants within biomes instead of uniform blocks.
	 */
	class SurfaceDecorator {
	public:
		static constexpr int SeaLevel = 62;

		explicit SurfaceDecorator(const uint64_t seed)
			: m_VegetationNoise(seed)
			, m_FlowerNoise(seed + 1)
			, m_SurfaceNoise(seed + 2)
			, m_PatchNoise(seed + 3) {}

		/**
		 * Get the surface block for a position, with noise-based variation.
		 */
		int32_t GetSurfaceBlock(const int worldX, const int worldZ,
			const int surfaceHeight, const BiomeType biome) const {

			if (surfaceHeight < SeaLevel - 2) return BlockState::Gravel;
			if (surfaceHeight <= SeaLevel + 1 && surfaceHeight >= SeaLevel - 2) return BlockState::Sand;

			const double blockX = static_cast<double>(worldX);
			const double blockZ = static_cast<double>(worldZ);
			const double surfaceSample = m_SurfaceNoise.Noise(blockX * 0.08, blockZ * 0.08);
			const double patchSample = m_PatchNoise.Noise(blockX * 0.15, blockZ * 0.15);

			switch (biome) {
				case BiomeType::Desert:
					// Occasional gravel/sandstone patches in desert
					if (surfaceSample > 0.7) return BlockState::Sandstone;
					return BlockState::Sand;

				case BiomeType::Badlands:
					// Layered terracotta with red sand patches
					if (patchSample > 0.6) return BlockState::RedSand;
					return BlockState::Terracotta;

				case BiomeType::Taiga:
					// Mix of podzol, coarse dirt, and mossy patches
					if (surfaceSample > 0.6) return BlockState::CoarseDirt;
					if (patchSample > 0.7) return BlockState::Dirt;
					return BlockState::Podzol;

				case BiomeType::Swamp:
					// Mud with dirt and grass patches
					if (surfaceSample > 0.5) return BlockState::Dirt;
					if (patchSample > 0.6) return BlockState::GrassBlock;
					return BlockState::Mud;

				case BiomeType::Mountain:
					// Stone with gravel and dirt patches
					if (surfaceSample > 0.5 && surfaceHeight < 100) return BlockState::GrassBlock;
					if (patchSample > 0.6) return BlockState::Gravel;
					return BlockState::Stone;

				case BiomeType::Jungle:
					// Dense grass with coarse dirt patches
					if (patchSample > 0.75) return BlockState::CoarseDirt;
					return BlockState::GrassBlock;

				case BiomeType::Savanna:
					// Grass with coarse dirt patches
					if (surfaceSample > 0.65) return BlockState::CoarseDirt;
					return BlockState::GrassBlock;

				case BiomeType::Plains:
				case BiomeType::Meadow:
					// Mostly grass with occasional dirt patches
					if (surfaceSample > 0.8) return BlockState::CoarseDirt;
					return BlockState::GrassBlock;

				case BiomeType::Forest:
				case BiomeType::DarkForest:
					// Grass with coarse dirt under trees
					if (patchSample > 0.65) return BlockState::CoarseDirt;
					return BlockState::GrassBlock;

				case BiomeType::BirchForest:
					return BlockState::GrassBlock;

				case BiomeType::SnowyPlains:
					// Grass (snow placed separately on top)
					if (surfaceSample > 0.75) return BlockState::CoarseDirt;
					return BlockState::GrassBlock;

				default:
					return BlockState::GrassBlock;
			}
		}

		/**
		 * Overload for callers without world position (backwards compat).
		 */
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

		/**
		 * Get sub-surface block with noise variation.
		 */
		int32_t GetSubSurfaceBlock(const int worldX, const int worldZ,
			const int surfaceHeight, const BiomeType biome) const {

			if (surfaceHeight <= SeaLevel + 1) return BlockState::Sand;

			const double blockX = static_cast<double>(worldX);
			const double blockZ = static_cast<double>(worldZ);
			const double subsurfaceSample = m_SurfaceNoise.Noise(blockX * 0.06 + 500, blockZ * 0.06 + 500);

			switch (biome) {
				case BiomeType::Desert:
					return BlockState::Sandstone;

				case BiomeType::Badlands:
					return BlockState::Terracotta;

				case BiomeType::Mountain:
					if (subsurfaceSample > 0.5) return BlockState::Gravel;
					return BlockState::Stone;

				default:
					// Occasional gravel layer under dirt
					if (subsurfaceSample > 0.8) return BlockState::Gravel;
					return BlockState::Dirt;
			}
		}

		/**
		 * Overload without world position (backwards compat).
		 */
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

			if (biome == BiomeType::Desert) {
				return BlockState::Air; // No grass in desert
			}

			const double vegetationSample = m_VegetationNoise.Noise(blockX * 0.5 + 500, blockZ * 0.5 + 500);
			const double grassThreshold = 1.0 - GetGrassDensity(biome);

			if (vegetationSample <= grassThreshold) {
				return BlockState::Air;
			}

			const double flowerSample = m_FlowerNoise.Noise(blockX * 1.3, blockZ * 1.3);

			// Biome-specific vegetation
			switch (biome) {
				case BiomeType::Meadow:
					// More flowers in meadows
					if (flowerSample > 0.5) {
						return (flowerSample > 0.75) ? BlockState::Poppy : BlockState::Dandelion;
					}
					return BlockState::ShortGrass;

				case BiomeType::Taiga:
					return BlockState::Fern;

				case BiomeType::Jungle:
					return (flowerSample > 0.8) ? BlockState::Fern : BlockState::ShortGrass;

				case BiomeType::Swamp:
					return BlockState::ShortGrass;

				default:
					if (flowerSample > 0.85) {
						return (flowerSample > 0.92) ? BlockState::Poppy : BlockState::Dandelion;
					}
					return BlockState::ShortGrass;
			}
		}

	private:
		static double GetGrassDensity(const BiomeType biome) {
			switch (biome) {
				case BiomeType::Plains:      return 0.35;
				case BiomeType::Meadow:      return 0.55;
				case BiomeType::Forest:
				case BiomeType::BirchForest: return 0.3;
				case BiomeType::DarkForest:  return 0.2;
				case BiomeType::Taiga:       return 0.25;
				case BiomeType::Jungle:      return 0.5;
				case BiomeType::Swamp:       return 0.35;
				case BiomeType::Savanna:     return 0.15;
				default:                     return 0.05;
			}
		}

		PerlinNoise m_VegetationNoise;
		PerlinNoise m_FlowerNoise;
		PerlinNoise m_SurfaceNoise;
		PerlinNoise m_PatchNoise;
	};

}
