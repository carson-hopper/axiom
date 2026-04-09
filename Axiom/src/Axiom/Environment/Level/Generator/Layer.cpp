#include "axpch.h"
#include "Axiom/Environment/Level/Generator/Layer.h"

namespace Axiom {

	Layer::Layer(int64_t seedMixup)
		: m_Parent(nullptr)
		, m_BaseSeed(seedMixup * seedMixup * 6364136223846793005LL + 1442695040888963407LL) {}

	Layer::Layer(int64_t seedMixup, Ref<Layer> parent)
		: m_Parent(std::move(parent))
		, m_BaseSeed(seedMixup * seedMixup * 6364136223846793005LL + 1442695040888963407LL) {}

	void Layer::InitWorldSeed(int64_t worldSeed) {
		m_WorldSeed = worldSeed;

		if (m_Parent) {
			m_Parent->InitWorldSeed(worldSeed);
		}

		/**
		 * Mix the world seed with the base seed three times
		 * to produce a well-distributed layer-specific seed.
		 */
		m_WorldSeed = m_WorldSeed * m_WorldSeed * 6364136223846793005LL + 1442695040888963407LL;
		m_WorldSeed += m_BaseSeed;
		m_WorldSeed = m_WorldSeed * m_WorldSeed * 6364136223846793005LL + 1442695040888963407LL;
		m_WorldSeed += m_BaseSeed;
		m_WorldSeed = m_WorldSeed * m_WorldSeed * 6364136223846793005LL + 1442695040888963407LL;
		m_WorldSeed += m_BaseSeed;
	}

	void Layer::InitCoordSeed(int x, int z) {
		m_CoordSeed = m_WorldSeed;
		m_CoordSeed = m_CoordSeed * m_CoordSeed * 6364136223846793005LL + 1442695040888963407LL;
		m_CoordSeed += x;
		m_CoordSeed = m_CoordSeed * m_CoordSeed * 6364136223846793005LL + 1442695040888963407LL;
		m_CoordSeed += z;
		m_CoordSeed = m_CoordSeed * m_CoordSeed * 6364136223846793005LL + 1442695040888963407LL;
		m_CoordSeed += x;
		m_CoordSeed = m_CoordSeed * m_CoordSeed * 6364136223846793005LL + 1442695040888963407LL;
		m_CoordSeed += z;
	}

	int Layer::NextInt(int bound) {
		/**
		 * Advance the coordinate seed and produce a value
		 * in [0, bound) using the same LCG constants as
		 * the Minecraft classic layer system.
		 */
		int result = static_cast<int>((m_CoordSeed >> 24) % bound);
		if (result < 0) {
			result += bound;
		}
		m_CoordSeed = m_CoordSeed * m_CoordSeed * 6364136223846793005LL + 1442695040888963407LL;
		m_CoordSeed += m_WorldSeed;
		return result;
	}

	int Layer::SelectRandom(int a, int b, int c, int d) {
		int choice = NextInt(4);
		switch (choice) {
			case 0: return a;
			case 1: return b;
			case 2: return c;
			case 3: return d;
			default: return a;
		}
	}

}
