#pragma once

#include "Axiom/Environment/Level/Generator/Layer.h"
#include "Axiom/Environment/Level/Generator/Biome.h"

namespace Axiom {

	/**
	 * Seed layer that produces the initial continent/ocean mask.
	 * Returns 1 (land) roughly 10% of the time, 0 (ocean) otherwise.
	 * The origin chunk (0,0) is always forced to land.
	 */
	class IslandLayer : public Layer {
	public:
		explicit IslandLayer(int64_t seedMixup);
		std::vector<int> GetArea(int areaX, int areaZ, int width, int height) override;
	};

	/**
	 * Doubles the resolution of the parent layer by interpolating
	 * each parent cell into a 2x2 block of output cells. Uses
	 * fuzzy random selection for natural-looking boundaries.
	 */
	class ZoomLayer : public Layer {
	public:
		ZoomLayer(int64_t seedMixup, Ref<Layer> parent);
		std::vector<int> GetArea(int areaX, int areaZ, int width, int height) override;
	};

	/**
	 * Randomly converts ocean tiles that are adjacent to land
	 * into land, growing continents outward.
	 */
	class AddIslandLayer : public Layer {
	public:
		AddIslandLayer(int64_t seedMixup, Ref<Layer> parent);
		std::vector<int> GetArea(int areaX, int areaZ, int width, int height) override;
	};

	/**
	 * Replaces generic land values (1) with random biome IDs.
	 * Ocean (0) remains unchanged.
	 */
	class BiomeLayer : public Layer {
	public:
		BiomeLayer(int64_t seedMixup, Ref<Layer> parent);
		std::vector<int> GetArea(int areaX, int areaZ, int width, int height) override;
	};

	/**
	 * Places Beach biome between land biomes and Ocean, and
	 * occasionally inserts River between different biome types.
	 */
	class ShoreLayer : public Layer {
	public:
		ShoreLayer(int64_t seedMixup, Ref<Layer> parent);
		std::vector<int> GetArea(int areaX, int areaZ, int width, int height) override;

	private:
		static bool IsOcean(int biomeId);
	};

	/**
	 * Removes single-cell noise by replacing values that are
	 * surrounded by a single other value with that value.
	 */
	class SmoothLayer : public Layer {
	public:
		SmoothLayer(int64_t seedMixup, Ref<Layer> parent);
		std::vector<int> GetArea(int areaX, int areaZ, int width, int height) override;
	};

}
