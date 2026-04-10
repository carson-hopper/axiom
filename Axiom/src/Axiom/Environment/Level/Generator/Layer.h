#pragma once

#include "Axiom/Core/Base.h"

#include <cstdint>
#include <vector>

namespace Axiom {

	/**
	 * Abstract base for biome generation layers. Each layer transforms
	 * an area of biome IDs, optionally reading from a parent layer.
	 * Seed mixing ensures deterministic results per world seed.
	 */
	class Layer : public virtual RefCounted {
	public:
		explicit Layer(int64_t seedMixup);
		Layer(int64_t seedMixup, Ref<Layer> parent);
		virtual ~Layer() = default;

		virtual std::vector<int> GetArea(int areaX, int areaZ, int width, int height) = 0;

		void InitWorldSeed(int64_t worldSeed);

	protected:
		void InitCoordSeed(int x, int z);
		int NextInt(int bound);
		int SelectRandom(int a, int b, int c, int d);

		Ref<Layer> m_Parent;
		int64_t m_WorldSeed = 0;
		int64_t m_CoordSeed = 0;
		int64_t m_BaseSeed;
	};

}
