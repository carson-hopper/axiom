#pragma once

#include "Axiom/Environment/Level/Generator/BiomeProvider.h"
#include "Axiom/Environment/Level/Generator/BlockStates.h"
#include "Axiom/Environment/Level/Generator/Noise.h"

#include <array>
#include <cmath>
#include <cstdint>
#include <random>
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
	 * Decorates underwater and waterside areas using vanilla-matched
	 * attempt counts and probabilities from 26.1 placed_feature data.
	 */
	class WaterDecorator {
	public:
		static constexpr int SeaLevel = 62;
		static constexpr int MinY = -64;
		static constexpr int MaxY = 319;
		static constexpr int WorldHeight = MaxY - MinY + 1;

		explicit WaterDecorator(const uint64_t seed)
			: m_Seed(seed)
			, m_KelpNoise(seed + 1) {}

		void Decorate(const int32_t chunkX, const int32_t chunkZ,
			const std::array<int, 256>& surfaceHeightmap,
			const std::array<BiomeType, 256>& biomeMap,
			std::vector<int32_t>& columnBlocks) const {

			// Deterministic RNG per chunk
			const uint64_t chunkSeed = m_Seed ^ (static_cast<uint64_t>(chunkX) * 341873128712ULL
				+ static_cast<uint64_t>(chunkZ) * 132897987541ULL);
			std::mt19937 random(chunkSeed);

			// Determine dominant biome for feature selection
			const BiomeType dominantBiome = biomeMap[128];

			PlaceSeagrass(random, chunkX, chunkZ, surfaceHeightmap, dominantBiome, columnBlocks);
			PlaceKelp(random, chunkX, chunkZ, surfaceHeightmap, dominantBiome, columnBlocks);
			PlaceClayDisks(random, chunkX, chunkZ, surfaceHeightmap, columnBlocks);
			PlaceSugarCane(random, chunkX, chunkZ, surfaceHeightmap, dominantBiome, columnBlocks);
			PlaceLilyPads(random, chunkX, chunkZ, surfaceHeightmap, dominantBiome, columnBlocks);
		}

	private:
		// ----- Seagrass (vanilla: 32-80 attempts per chunk) -------------

		void PlaceSeagrass(std::mt19937& random, const int32_t chunkX, const int32_t chunkZ,
			const std::array<int, 256>& surfaceHeightmap, const BiomeType biome,
			std::vector<int32_t>& columnBlocks) const {

			int attemptCount;
			double tallProbability;

			switch (biome) {
				case BiomeType::Swamp:
					attemptCount = 64; tallProbability = 0.6; break;
				case BiomeType::Ocean:
					attemptCount = 48; tallProbability = 0.3; break;
				default:
					attemptCount = 32; tallProbability = 0.3; break;
			}

			std::uniform_int_distribution<int> positionDistribution(0, 15);
			std::uniform_real_distribution<double> probabilityDistribution(0.0, 1.0);

			for (int attempt = 0; attempt < attemptCount; attempt++) {
				const int localX = positionDistribution(random);
				const int localZ = positionDistribution(random);
				const int columnIndex = localZ * 16 + localX;
				const int surfaceHeight = surfaceHeightmap[columnIndex];
				const int waterDepth = SeaLevel - surfaceHeight;

				if (waterDepth < 2 || waterDepth > 25) continue;

				const int aboveFloorY = surfaceHeight - MinY + 1;
				if (columnBlocks[aboveFloorY * 256 + columnIndex] != BlockState::Water) continue;

				// Vanilla probability determines if this seagrass is "tall" (we just use short)
				if (probabilityDistribution(random) < tallProbability) {
					columnBlocks[aboveFloorY * 256 + columnIndex] = BlockState::Seagrass;
				} else {
					columnBlocks[aboveFloorY * 256 + columnIndex] = BlockState::Seagrass;
				}
			}
		}

		// ----- Kelp (vanilla: noise_based_count 80-120) -----------------

		void PlaceKelp(std::mt19937& random, const int32_t chunkX, const int32_t chunkZ,
			const std::array<int, 256>& surfaceHeightmap, const BiomeType biome,
			std::vector<int32_t>& columnBlocks) const {

			if (biome != BiomeType::Ocean) return;

			// Vanilla: noise_based_count with noise_to_count_ratio=80-120, noise_factor=80
			const double noiseValue = m_KelpNoise.Noise(chunkX * 0.0578, chunkZ * 0.0578);
			const int attemptCount = static_cast<int>(std::max(0.0, noiseValue * 80.0 + 100.0));

			std::uniform_int_distribution<int> positionDistribution(0, 15);
			std::uniform_int_distribution<int> heightVariation(2, 25);

			for (int attempt = 0; attempt < attemptCount; attempt++) {
				const int localX = positionDistribution(random);
				const int localZ = positionDistribution(random);
				const int columnIndex = localZ * 16 + localX;
				const int surfaceHeight = surfaceHeightmap[columnIndex];
				const int waterDepth = SeaLevel - surfaceHeight;

				if (waterDepth < 4) continue;

				const int kelpHeight = std::min(heightVariation(random), waterDepth - 1);

				for (int heightOffset = 1; heightOffset <= kelpHeight; heightOffset++) {
					const int absoluteY = surfaceHeight - MinY + heightOffset;
					if (columnBlocks[absoluteY * 256 + columnIndex] == BlockState::Water) {
						columnBlocks[absoluteY * 256 + columnIndex] = BlockState::KelpPlant;
					} else {
						break;
					}
				}
			}
		}

		// ----- Clay disks (vanilla: 1 per chunk, radius 2-3) ------------

		void PlaceClayDisks(std::mt19937& random, const int32_t chunkX, const int32_t chunkZ,
			const std::array<int, 256>& surfaceHeightmap,
			std::vector<int32_t>& columnBlocks) const {

			std::uniform_int_distribution<int> positionDistribution(0, 15);
			std::uniform_int_distribution<int> radiusDistribution(2, 3);

			// 1 clay disk per chunk
			const int centerLocalX = positionDistribution(random);
			const int centerLocalZ = positionDistribution(random);
			const int centerColumnIndex = centerLocalZ * 16 + centerLocalX;
			const int centerSurfaceHeight = surfaceHeightmap[centerColumnIndex];

			if (centerSurfaceHeight >= SeaLevel) return;

			const int radius = radiusDistribution(random);

			for (int offsetX = -radius; offsetX <= radius; offsetX++) {
				for (int offsetZ = -radius; offsetZ <= radius; offsetZ++) {
					if (offsetX * offsetX + offsetZ * offsetZ > radius * radius) continue;

					const int localX = centerLocalX + offsetX;
					const int localZ = centerLocalZ + offsetZ;
					if (localX < 0 || localX >= 16 || localZ < 0 || localZ >= 16) continue;

					const int columnIndex = localZ * 16 + localX;
					const int surfaceHeight = surfaceHeightmap[columnIndex];
					if (surfaceHeight >= SeaLevel) continue;

					// Replace surface and 1-2 below with clay
					for (int depth = 0; depth <= 2; depth++) {
						const int absoluteY = surfaceHeight - MinY - depth;
						if (absoluteY < 0) break;
						const int32_t existing = columnBlocks[absoluteY * 256 + columnIndex];
						if (existing == BlockState::Dirt || existing == BlockState::Sand
							|| existing == BlockState::Gravel || existing == BlockState::Clay) {
							columnBlocks[absoluteY * 256 + columnIndex] = BlockState::Clay;
						}
					}
				}
			}
		}

		// ----- Sugar cane (vanilla: rarity 1/6, 20 attempts, height 2-4)

		void PlaceSugarCane(std::mt19937& random, const int32_t chunkX, const int32_t chunkZ,
			const std::array<int, 256>& surfaceHeightmap, const BiomeType biome,
			std::vector<int32_t>& columnBlocks) const {

			// Vanilla rarity filter: 1/3 for swamp, 1/6 for normal, always for desert
			int rarityDenominator;
			switch (biome) {
				case BiomeType::Desert:  rarityDenominator = 1; break;
				case BiomeType::Swamp:   rarityDenominator = 3; break;
				case BiomeType::Badlands: rarityDenominator = 5; break;
				default:                 rarityDenominator = 6; break;
			}

			std::uniform_int_distribution<int> rarityCheck(1, rarityDenominator);
			if (rarityCheck(random) != 1) return;

			std::uniform_int_distribution<int> positionDistribution(0, 15);
			std::uniform_int_distribution<int> spreadDistribution(-4, 4);
			std::uniform_int_distribution<int> heightDistribution(2, 4);

			// 20 attempts spread around a center point
			const int centerX = positionDistribution(random);
			const int centerZ = positionDistribution(random);

			for (int attempt = 0; attempt < 20; attempt++) {
				const int localX = std::clamp(centerX + spreadDistribution(random), 0, 15);
				const int localZ = std::clamp(centerZ + spreadDistribution(random), 0, 15);
				const int columnIndex = localZ * 16 + localX;
				const int surfaceHeight = surfaceHeightmap[columnIndex];

				if (surfaceHeight < SeaLevel - 1 || surfaceHeight > SeaLevel + 1) continue;

				const int surfaceAbsoluteY = surfaceHeight - MinY;
				const int aboveAbsoluteY = surfaceAbsoluteY + 1;
				if (aboveAbsoluteY >= WorldHeight) continue;
				if (columnBlocks[aboveAbsoluteY * 256 + columnIndex] != BlockState::Air) continue;

				const int32_t surfaceBlock = columnBlocks[surfaceAbsoluteY * 256 + columnIndex];
				if (surfaceBlock != BlockState::Sand && surfaceBlock != BlockState::Dirt
					&& surfaceBlock != BlockState::GrassBlock) continue;

				const int caneHeight = heightDistribution(random);
				for (int heightOffset = 1; heightOffset <= caneHeight; heightOffset++) {
					const int caneAbsoluteY = surfaceAbsoluteY + heightOffset;
					if (caneAbsoluteY >= WorldHeight) break;
					if (columnBlocks[caneAbsoluteY * 256 + columnIndex] != BlockState::Air) break;
					columnBlocks[caneAbsoluteY * 256 + columnIndex] = BlockState::SugarCane;
				}
			}
		}

		// ----- Lily pads (vanilla: 4 × 10 attempts in swamp) ------------

		void PlaceLilyPads(std::mt19937& random, const int32_t chunkX, const int32_t chunkZ,
			const std::array<int, 256>& surfaceHeightmap, const BiomeType biome,
			std::vector<int32_t>& columnBlocks) const {

			if (biome != BiomeType::Swamp) return;

			std::uniform_int_distribution<int> positionDistribution(0, 15);
			std::uniform_int_distribution<int> spreadDistribution(-7, 7);

			// Vanilla: 4 center points, each with 10 spread attempts
			for (int center = 0; center < 4; center++) {
				const int centerX = positionDistribution(random);
				const int centerZ = positionDistribution(random);

				for (int attempt = 0; attempt < 10; attempt++) {
					const int localX = std::clamp(centerX + spreadDistribution(random), 0, 15);
					const int localZ = std::clamp(centerZ + spreadDistribution(random), 0, 15);
					const int columnIndex = localZ * 16 + localX;

					// Lily pad goes on top of water at sea level
					const int seaAbsoluteY = SeaLevel - MinY;
					const int aboveSeaAbsoluteY = seaAbsoluteY + 1;

					if (columnBlocks[seaAbsoluteY * 256 + columnIndex] == BlockState::Water
						&& columnBlocks[aboveSeaAbsoluteY * 256 + columnIndex] == BlockState::Air) {
						columnBlocks[aboveSeaAbsoluteY * 256 + columnIndex] = BlockState::LilyPad;
					}
				}
			}
		}

		uint64_t m_Seed;
		PerlinNoise m_KelpNoise;
	};

}
