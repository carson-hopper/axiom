#pragma once

#include <cstdint>

namespace Axiom {

	/**
	 * Classic Perlin noise following MCLCE's ImprovedNoise.
	 * Produces smooth, continuous noise values for terrain generation
	 * using the standard improved Perlin algorithm with fade curve
	 * t^3(6t^2 - 15t + 10).
	 */
	class ClassicPerlinNoise {
	public:
		explicit ClassicPerlinNoise(int64_t seed);

		double Noise2D(double x, double z) const;
		double Noise3D(double x, double y, double z) const;
		double OctaveNoise2D(double x, double z, int octaves, double persistence = 0.5) const;
		double OctaveNoise3D(double x, double y, double z, int octaves, double persistence = 0.5) const;

	private:
		static double Fade(double t);
		static double Lerp(double t, double a, double b);
		static double Grad(int hash, double x, double y, double z);
		static double Grad2D(int hash, double x, double z);

		int m_Permutation[512];
		double m_OffsetX;
		double m_OffsetY;
		double m_OffsetZ;
	};

}
