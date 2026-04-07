#pragma once

#include "Axiom/Environment/World/Generator/BiomeProvider.h"
#include "Axiom/Environment/World/Generator/BlockStates.h"
#include "Axiom/Environment/World/Generator/Noise.h"

#include <array>
#include <cmath>
#include <cstdint>
#include <vector>

namespace Axiom {

	namespace BlockState {
		constexpr int32_t Seagrass = 2254;
		constexpr int32_t Clay = 6946;
		constexpr int32_t SugarCane = 6947;
		constexpr int32_t LilyPad = 8920;
		constexpr int32_t KelpPlant = 15088;
	}

	/**
	 * Decorates underwater and waterside areas with seagrass, kelp,
	 * clay patches, sugar cane, and lily pads.
	 */
	class WaterDecorator {
	public:
		static constexpr int SeaLevel = 62;
		static constexpr int MinY = -64;
		static constexpr int MaxY = 319;

		explicit WaterDecorator(const uint64_t seed)
			: m_SeagrassNoise(seed)
			, m_KelpNoise(seed + 1)
			, m_ClayNoise(seed + 2)
			, m_WatersideNoise(seed + 3) {}

		/**
		 * Place underwater decorations: seagrass, kelp, clay patches.
		 * Place waterside decorations: sugar cane, lily pads.
		 */
		void Decorate(const int32_t chunkX, const int32_t chunkZ,
			const std::array<int, 256>& surfaceHeightmap,
			const std::array<BiomeType, 256>& biomeMap,
			std::vector<int32_t>& columnBlocks) const {

			for (int localZ = 0; localZ < 16; localZ++) {
				for (int localX = 0; localX < 16; localX++) {
					const int worldX = chunkX * 16 + localX;
					const int worldZ = chunkZ * 16 + localZ;
					const int columnIndex = localZ * 16 + localX;
					const int surfaceHeight = surfaceHeightmap[columnIndex];
					const BiomeType biome = biomeMap[columnIndex];

					if (surfaceHeight < SeaLevel - 1) {
						PlaceUnderwaterFeatures(worldX, worldZ, surfaceHeight, biome,
							columnIndex, columnBlocks);
					} else if (surfaceHeight >= SeaLevel - 1 && surfaceHeight <= SeaLevel + 2) {
						PlaceWatersideFeatures(worldX, worldZ, surfaceHeight, biome,
							columnIndex, columnBlocks);
					}
				}
			}
		}

	private:
		void PlaceUnderwaterFeatures(const int worldX, const int worldZ,
			const int surfaceHeight, const BiomeType biome,
			const int columnIndex, std::vector<int32_t>& columnBlocks) const {

			const double blockX = static_cast<double>(worldX);
			const double blockZ = static_cast<double>(worldZ);
			const int waterDepth = SeaLevel - surfaceHeight;

			// Clay patches on ocean/river floor
			const double claySample = m_ClayNoise.Noise(blockX * 0.1, blockZ * 0.1);
			if (claySample > 0.6) {
				const int surfaceAbsoluteY = surfaceHeight - MinY;
				const int32_t existingBlock = columnBlocks[surfaceAbsoluteY * 256 + columnIndex];
				if (existingBlock == BlockState::Gravel || existingBlock == BlockState::Sand
					|| existingBlock == BlockState::Dirt) {
					columnBlocks[surfaceAbsoluteY * 256 + columnIndex] = BlockState::Clay;
				}
			}

			// Seagrass on shallow ocean floor
			if (waterDepth < 15 && waterDepth > 1) {
				const double seagrassSample = m_SeagrassNoise.Noise(blockX * 0.3, blockZ * 0.3);
				const double seagrassThreshold = (biome == BiomeType::Swamp) ? 0.3 : 0.55;

				if (seagrassSample > seagrassThreshold) {
					const int aboveFloorAbsoluteY = surfaceHeight - MinY + 1;
					if (columnBlocks[aboveFloorAbsoluteY * 256 + columnIndex] == BlockState::Water) {
						columnBlocks[aboveFloorAbsoluteY * 256 + columnIndex] = BlockState::Seagrass;
					}
				}
			}

			// Kelp in deeper ocean water
			if (waterDepth >= 5 && biome == BiomeType::Ocean) {
				const double kelpSample = m_KelpNoise.Noise(blockX * 0.2, blockZ * 0.2);
				if (kelpSample > 0.6) {
					// Kelp grows from floor upward as a column of KelpPlant blocks
					const int kelpHeight = static_cast<int>((kelpSample - 0.6) * 25.0);
					const int maxKelpHeight = std::min(kelpHeight, waterDepth - 2);

					for (int heightOffset = 1; heightOffset <= maxKelpHeight; heightOffset++) {
						const int kelpAbsoluteY = surfaceHeight - MinY + heightOffset;
						if (columnBlocks[kelpAbsoluteY * 256 + columnIndex] == BlockState::Water) {
							columnBlocks[kelpAbsoluteY * 256 + columnIndex] = BlockState::KelpPlant;
						}
					}
				}
			}
		}

		void PlaceWatersideFeatures(const int worldX, const int worldZ,
			const int surfaceHeight, const BiomeType biome,
			const int columnIndex, std::vector<int32_t>& columnBlocks) const {

			const double blockX = static_cast<double>(worldX);
			const double blockZ = static_cast<double>(worldZ);
			const double watersideSample = m_WatersideNoise.Noise(blockX * 0.4, blockZ * 0.4);

			const int surfaceAbsoluteY = surfaceHeight - MinY;
			const int aboveAbsoluteY = surfaceAbsoluteY + 1;
			if (aboveAbsoluteY >= MaxY - MinY) return;

			const int32_t aboveBlock = columnBlocks[aboveAbsoluteY * 256 + columnIndex];

			// Lily pads on water surface in swamps
			if (biome == BiomeType::Swamp && aboveBlock == BlockState::Water) {
				if (watersideSample > 0.5) {
					// Check if this is actually the water surface (air above)
					const int twoAboveAbsoluteY = aboveAbsoluteY + 1;
					if (twoAboveAbsoluteY < MaxY - MinY
						&& columnBlocks[twoAboveAbsoluteY * 256 + columnIndex] == BlockState::Air) {
						columnBlocks[twoAboveAbsoluteY * 256 + columnIndex] = BlockState::LilyPad;
					}
				}
			}

			// Sugar cane next to water on sand/dirt/grass
			if (aboveBlock == BlockState::Air && surfaceHeight == SeaLevel) {
				const int32_t surfaceBlock = columnBlocks[surfaceAbsoluteY * 256 + columnIndex];
				const bool canPlaceSugarCane = surfaceBlock == BlockState::Sand
					|| surfaceBlock == BlockState::Dirt || surfaceBlock == BlockState::GrassBlock;

				if (canPlaceSugarCane && watersideSample > 0.7) {
					// Sugar cane grows 1-3 blocks tall
					const int sugarCaneHeight = 1 + static_cast<int>((watersideSample - 0.7) * 10.0);
					const int maxHeight = std::min(sugarCaneHeight, 3);

					for (int heightOffset = 1; heightOffset <= maxHeight; heightOffset++) {
						const int caneAbsoluteY = surfaceAbsoluteY + heightOffset;
						if (caneAbsoluteY >= MaxY - MinY) break;
						if (columnBlocks[caneAbsoluteY * 256 + columnIndex] != BlockState::Air) break;
						columnBlocks[caneAbsoluteY * 256 + columnIndex] = BlockState::SugarCane;
					}
				}
			}
		}

		PerlinNoise m_SeagrassNoise;
		PerlinNoise m_KelpNoise;
		PerlinNoise m_ClayNoise;
		PerlinNoise m_WatersideNoise;
	};

}
