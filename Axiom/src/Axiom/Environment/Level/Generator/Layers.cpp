#include "axpch.h"
#include "Axiom/Environment/Level/Generator/Layers.h"

namespace Axiom {

	// ---- IslandLayer ------------------------------------------------

	IslandLayer::IslandLayer(int64_t seedMixup)
		: Layer(seedMixup) {}

	std::vector<int> IslandLayer::GetArea(int areaX, int areaZ, int width, int height) {
		std::vector<int> result(width * height);

		for (int z = 0; z < height; z++) {
			for (int x = 0; x < width; x++) {
				InitCoordSeed(areaX + x, areaZ + z);

				/**
				 * Force the origin chunk to land so the player
				 * always spawns on solid ground; everywhere else
				 * is land only ~10% of the time.
				 */
				if (areaX + x == 0 && areaZ + z == 0) {
					result[z * width + x] = 1;
				} else {
					result[z * width + x] = (NextInt(10) == 0) ? 1 : 0;
				}
			}
		}

		return result;
	}

	// ---- ZoomLayer --------------------------------------------------

	ZoomLayer::ZoomLayer(int64_t seedMixup, Ref<Layer> parent)
		: Layer(seedMixup, std::move(parent)) {}

	std::vector<int> ZoomLayer::GetArea(int areaX, int areaZ, int width, int height) {
		/**
		 * Request the parent area at half resolution, with one extra
		 * cell on each axis for interpolation margin.
		 */
		int parentX = areaX >> 1;
		int parentZ = areaZ >> 1;
		int parentWidth = (width >> 1) + 2;
		int parentHeight = (height >> 1) + 2;

		std::vector<int> parentData = m_Parent->GetArea(parentX, parentZ, parentWidth, parentHeight);
		int zoomedWidth = (parentWidth - 1) * 2;
		int zoomedHeight = (parentHeight - 1) * 2;
		std::vector<int> zoomed(zoomedWidth * zoomedHeight);

		for (int parentZIndex = 0; parentZIndex < parentHeight - 1; parentZIndex++) {
			for (int parentXIndex = 0; parentXIndex < parentWidth - 1; parentXIndex++) {
				InitCoordSeed(
					(parentX + parentXIndex) * 2,
					(parentZ + parentZIndex) * 2
				);

				int topLeft = parentData[parentZIndex * parentWidth + parentXIndex];
				int topRight = parentData[parentZIndex * parentWidth + parentXIndex + 1];
				int bottomLeft = parentData[(parentZIndex + 1) * parentWidth + parentXIndex];
				int bottomRight = parentData[(parentZIndex + 1) * parentWidth + parentXIndex + 1];

				int outputX = parentXIndex * 2;
				int outputZ = parentZIndex * 2;

				/**
				 * Each parent cell expands into 2x2 output cells:
				 * top-left copies directly, edges pick randomly
				 * from their two neighbors, center picks from all four.
				 */
				zoomed[outputZ * zoomedWidth + outputX] = topLeft;
				zoomed[(outputZ + 1) * zoomedWidth + outputX] = (NextInt(2) == 0) ? topLeft : bottomLeft;
				zoomed[outputZ * zoomedWidth + outputX + 1] = (NextInt(2) == 0) ? topLeft : topRight;
				zoomed[(outputZ + 1) * zoomedWidth + outputX + 1] = SelectRandom(topLeft, topRight, bottomLeft, bottomRight);
			}
		}

		/**
		 * Crop the zoomed result to the requested area,
		 * accounting for the offset within the zoomed buffer.
		 */
		int offsetX = areaX & 1;
		int offsetZ = areaZ & 1;
		std::vector<int> result(width * height);

		for (int z = 0; z < height; z++) {
			for (int x = 0; x < width; x++) {
				result[z * width + x] = zoomed[(z + offsetZ) * zoomedWidth + (x + offsetX)];
			}
		}

		return result;
	}

	// ---- AddIslandLayer ---------------------------------------------

	AddIslandLayer::AddIslandLayer(int64_t seedMixup, Ref<Layer> parent)
		: Layer(seedMixup, std::move(parent)) {}

	std::vector<int> AddIslandLayer::GetArea(int areaX, int areaZ, int width, int height) {
		/**
		 * Request a one-cell border around the target area so
		 * we can inspect neighbors for each output cell.
		 */
		std::vector<int> parentData = m_Parent->GetArea(areaX - 1, areaZ - 1, width + 2, height + 2);
		std::vector<int> result(width * height);

		int parentWidth = width + 2;

		for (int z = 0; z < height; z++) {
			for (int x = 0; x < width; x++) {
				int center = parentData[(z + 1) * parentWidth + (x + 1)];
				int north = parentData[z * parentWidth + (x + 1)];
				int south = parentData[(z + 2) * parentWidth + (x + 1)];
				int west = parentData[(z + 1) * parentWidth + x];
				int east = parentData[(z + 1) * parentWidth + (x + 2)];

				InitCoordSeed(areaX + x, areaZ + z);

				/**
				 * If the center is ocean but at least one neighbor
				 * is land, there is a 1-in-3 chance of converting
				 * this cell to land.
				 */
				if (center == 0 && (north != 0 || south != 0 || west != 0 || east != 0)) {
					result[z * width + x] = (NextInt(3) == 0) ? 1 : 0;
				} else {
					result[z * width + x] = center;
				}
			}
		}

		return result;
	}

	// ---- BiomeLayer -------------------------------------------------

	BiomeLayer::BiomeLayer(int64_t seedMixup, Ref<Layer> parent)
		: Layer(seedMixup, std::move(parent)) {}

	std::vector<int> BiomeLayer::GetArea(int areaX, int areaZ, int width, int height) {
		std::vector<int> parentData = m_Parent->GetArea(areaX, areaZ, width, height);
		std::vector<int> result(width * height);

		/**
		 * The set of land biomes that can be randomly assigned.
		 * Ocean (0) passes through unchanged.
		 */
		static constexpr int landBiomes[] = {
			BiomeRegistry::PLAINS,
			BiomeRegistry::DESERT,
			BiomeRegistry::FOREST,
			BiomeRegistry::TAIGA,
			BiomeRegistry::MOUNTAINS,
			BiomeRegistry::JUNGLE,
			BiomeRegistry::SWAMP,
			BiomeRegistry::SAVANNA,
			BiomeRegistry::BADLANDS,
			BiomeRegistry::ICE_PLAINS
		};
		static constexpr int landBiomeCount = sizeof(landBiomes) / sizeof(landBiomes[0]);

		for (int z = 0; z < height; z++) {
			for (int x = 0; x < width; x++) {
				InitCoordSeed(areaX + x, areaZ + z);

				int value = parentData[z * width + x];
				if (value == 0) {
					result[z * width + x] = BiomeRegistry::OCEAN;
				} else {
					result[z * width + x] = landBiomes[NextInt(landBiomeCount)];
				}
			}
		}

		return result;
	}

	// ---- ShoreLayer -------------------------------------------------

	ShoreLayer::ShoreLayer(int64_t seedMixup, Ref<Layer> parent)
		: Layer(seedMixup, std::move(parent)) {}

	bool ShoreLayer::IsOcean(int biomeId) {
		return biomeId == BiomeRegistry::OCEAN || biomeId == BiomeRegistry::FROZEN_OCEAN;
	}

	std::vector<int> ShoreLayer::GetArea(int areaX, int areaZ, int width, int height) {
		std::vector<int> parentData = m_Parent->GetArea(areaX - 1, areaZ - 1, width + 2, height + 2);
		std::vector<int> result(width * height);

		int parentWidth = width + 2;

		for (int z = 0; z < height; z++) {
			for (int x = 0; x < width; x++) {
				int center = parentData[(z + 1) * parentWidth + (x + 1)];
				int north = parentData[z * parentWidth + (x + 1)];
				int south = parentData[(z + 2) * parentWidth + (x + 1)];
				int west = parentData[(z + 1) * parentWidth + x];
				int east = parentData[(z + 1) * parentWidth + (x + 2)];

				InitCoordSeed(areaX + x, areaZ + z);

				/**
				 * Place Beach between any land biome and Ocean.
				 * Insert River sometimes between two different
				 * non-ocean biome types for variety.
				 */
				if (!IsOcean(center)) {
					if (IsOcean(north) || IsOcean(south) || IsOcean(west) || IsOcean(east)) {
						result[z * width + x] = BiomeRegistry::BEACH;
					} else if (center != north || center != south || center != west || center != east) {
						if (NextInt(5) == 0) {
							result[z * width + x] = BiomeRegistry::RIVER;
						} else {
							result[z * width + x] = center;
						}
					} else {
						result[z * width + x] = center;
					}
				} else {
					result[z * width + x] = center;
				}
			}
		}

		return result;
	}

	// ---- SmoothLayer ------------------------------------------------

	SmoothLayer::SmoothLayer(int64_t seedMixup, Ref<Layer> parent)
		: Layer(seedMixup, std::move(parent)) {}

	std::vector<int> SmoothLayer::GetArea(int areaX, int areaZ, int width, int height) {
		std::vector<int> parentData = m_Parent->GetArea(areaX - 1, areaZ - 1, width + 2, height + 2);
		std::vector<int> result(width * height);

		int parentWidth = width + 2;

		for (int z = 0; z < height; z++) {
			for (int x = 0; x < width; x++) {
				int center = parentData[(z + 1) * parentWidth + (x + 1)];
				int north = parentData[z * parentWidth + (x + 1)];
				int south = parentData[(z + 2) * parentWidth + (x + 1)];
				int west = parentData[(z + 1) * parentWidth + x];
				int east = parentData[(z + 1) * parentWidth + (x + 2)];

				/**
				 * If horizontal neighbors match and vertical neighbors match
				 * (but the two pairs differ), pick one pair randomly.
				 * If only one axis matches, adopt that neighbor.
				 * Otherwise keep the center value.
				 */
				if (west == east && north == south) {
					InitCoordSeed(areaX + x, areaZ + z);
					result[z * width + x] = (NextInt(2) == 0) ? west : north;
				} else if (west == east) {
					result[z * width + x] = west;
				} else if (north == south) {
					result[z * width + x] = north;
				} else {
					result[z * width + x] = center;
				}
			}
		}

		return result;
	}

}
