#pragma once

#include <cmath>
#include <cstdint>
#include <array>
#include <numeric>
#include <random>

namespace Axiom {

	/**
	 * Perlin noise implementation for terrain generation.
	 * Produces smooth, continuous noise values in the range [-1, 1].
	 */
	class PerlinNoise {
	public:
		explicit PerlinNoise(uint64_t seed = 0) {
			std::iota(m_Permutation.begin(), m_Permutation.end(), 0);
			std::mt19937_64 random(seed);
			std::shuffle(m_Permutation.begin(), m_Permutation.end(), random);
		}

		double Noise(double x, double z) const {
			int xi = static_cast<int>(std::floor(x)) & 255;
			int zi = static_cast<int>(std::floor(z)) & 255;

			double xf = x - std::floor(x);
			double zf = z - std::floor(z);

			double u = Fade(xf);
			double v = Fade(zf);

			int aa = m_Permutation[m_Permutation[xi] + zi];
			int ab = m_Permutation[m_Permutation[xi] + zi + 1];
			int ba = m_Permutation[m_Permutation[xi + 1] + zi];
			int bb = m_Permutation[m_Permutation[xi + 1] + zi + 1];

			double x1 = Lerp(Grad(aa, xf, zf), Grad(ba, xf - 1, zf), u);
			double x2 = Lerp(Grad(ab, xf, zf - 1), Grad(bb, xf - 1, zf - 1), u);

			return Lerp(x1, x2, v);
		}

		/**
		 * Octave noise — layer multiple frequencies for natural-looking terrain.
		 */
		double OctaveNoise(double x, double z, int octaves, double persistence = 0.5) const {
			double total = 0.0;
			double frequency = 1.0;
			double amplitude = 1.0;
			double maxValue = 0.0;

			for (int i = 0; i < octaves; i++) {
				total += Noise(x * frequency, z * frequency) * amplitude;
				maxValue += amplitude;
				amplitude *= persistence;
				frequency *= 2.0;
			}

			return total / maxValue;
		}

		/**
		 * 3D Perlin noise for cave generation.
		 */
		double Noise3D(double x, double y, double z) const {
			int xi = static_cast<int>(std::floor(x)) & 255;
			int yi = static_cast<int>(std::floor(y)) & 255;
			int zi = static_cast<int>(std::floor(z)) & 255;

			double xf = x - std::floor(x);
			double yf = y - std::floor(y);
			double zf = z - std::floor(z);

			double u = Fade(xf);
			double v = Fade(yf);
			double w = Fade(zf);

			int aaa = m_Permutation[m_Permutation[m_Permutation[xi] + yi] + zi];
			int aba = m_Permutation[m_Permutation[m_Permutation[xi] + yi + 1] + zi];
			int aab = m_Permutation[m_Permutation[m_Permutation[xi] + yi] + zi + 1];
			int abb = m_Permutation[m_Permutation[m_Permutation[xi] + yi + 1] + zi + 1];
			int baa = m_Permutation[m_Permutation[m_Permutation[xi + 1] + yi] + zi];
			int bba = m_Permutation[m_Permutation[m_Permutation[xi + 1] + yi + 1] + zi];
			int bab = m_Permutation[m_Permutation[m_Permutation[xi + 1] + yi] + zi + 1];
			int bbb = m_Permutation[m_Permutation[m_Permutation[xi + 1] + yi + 1] + zi + 1];

			double x1 = Lerp(Grad3D(aaa, xf, yf, zf), Grad3D(baa, xf - 1, yf, zf), u);
			double x2 = Lerp(Grad3D(aba, xf, yf - 1, zf), Grad3D(bba, xf - 1, yf - 1, zf), u);
			double y1 = Lerp(x1, x2, v);

			x1 = Lerp(Grad3D(aab, xf, yf, zf - 1), Grad3D(bab, xf - 1, yf, zf - 1), u);
			x2 = Lerp(Grad3D(abb, xf, yf - 1, zf - 1), Grad3D(bbb, xf - 1, yf - 1, zf - 1), u);
			double y2 = Lerp(x1, x2, v);

			return Lerp(y1, y2, w);
		}

	private:
		static double Fade(double t) {
			return t * t * t * (t * (t * 6 - 15) + 10);
		}

		static double Lerp(double a, double b, double t) {
			return a + t * (b - a);
		}

		static double Grad(int hash, double x, double z) {
			switch (hash & 3) {
				case 0: return x + z;
				case 1: return -x + z;
				case 2: return x - z;
				case 3: return -x - z;
				default: return 0;
			}
		}

		static double Grad3D(int hash, double x, double y, double z) {
			int h = hash & 15;
			double u = h < 8 ? x : y;
			double v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
			return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
		}

		std::array<int, 512> m_Permutation;
	};

}
