#include "axpch.h"
#include "Axiom/Environment/Level/Generator/PerlinNoise.h"

#include <cmath>
#include <random>

namespace Axiom {

	ClassicPerlinNoise::ClassicPerlinNoise(int64_t seed) {
		std::mt19937_64 random(static_cast<uint64_t>(seed));
		std::uniform_real_distribution<double> offsetDistribution(0.0, 256.0);

		m_OffsetX = offsetDistribution(random);
		m_OffsetY = offsetDistribution(random);
		m_OffsetZ = offsetDistribution(random);

		/**
		 * Fill the first 256 entries with identity values,
		 * then Fisher-Yates shuffle using the seeded RNG.
		 */
		for (int index = 0; index < 256; index++) {
			m_Permutation[index] = index;
		}

		for (int index = 255; index > 0; index--) {
			std::uniform_int_distribution<int> swapDistribution(0, index);
			int swapTarget = swapDistribution(random);
			int temporary = m_Permutation[index];
			m_Permutation[index] = m_Permutation[swapTarget];
			m_Permutation[swapTarget] = temporary;
		}

		/**
		 * Duplicate the first 256 entries into the second half
		 * so we never need to mask indices during lookups.
		 */
		for (int index = 0; index < 256; index++) {
			m_Permutation[index + 256] = m_Permutation[index];
		}
	}

	double ClassicPerlinNoise::Noise2D(double x, double z) const {
		x += m_OffsetX;
		z += m_OffsetZ;

		int floorX = static_cast<int>(std::floor(x)) & 255;
		int floorZ = static_cast<int>(std::floor(z)) & 255;

		double fractionalX = x - std::floor(x);
		double fractionalZ = z - std::floor(z);

		double fadeX = Fade(fractionalX);
		double fadeZ = Fade(fractionalZ);

		int hashAA = m_Permutation[m_Permutation[floorX] + floorZ];
		int hashAB = m_Permutation[m_Permutation[floorX] + floorZ + 1];
		int hashBA = m_Permutation[m_Permutation[floorX + 1] + floorZ];
		int hashBB = m_Permutation[m_Permutation[floorX + 1] + floorZ + 1];

		double gradAA = Grad2D(hashAA, fractionalX, fractionalZ);
		double gradBA = Grad2D(hashBA, fractionalX - 1.0, fractionalZ);
		double gradAB = Grad2D(hashAB, fractionalX, fractionalZ - 1.0);
		double gradBB = Grad2D(hashBB, fractionalX - 1.0, fractionalZ - 1.0);

		double lerpX1 = Lerp(fadeX, gradAA, gradBA);
		double lerpX2 = Lerp(fadeX, gradAB, gradBB);

		return Lerp(fadeZ, lerpX1, lerpX2);
	}

	double ClassicPerlinNoise::Noise3D(double x, double y, double z) const {
		x += m_OffsetX;
		y += m_OffsetY;
		z += m_OffsetZ;

		int floorX = static_cast<int>(std::floor(x)) & 255;
		int floorY = static_cast<int>(std::floor(y)) & 255;
		int floorZ = static_cast<int>(std::floor(z)) & 255;

		double fractionalX = x - std::floor(x);
		double fractionalY = y - std::floor(y);
		double fractionalZ = z - std::floor(z);

		double fadeX = Fade(fractionalX);
		double fadeY = Fade(fractionalY);
		double fadeZ = Fade(fractionalZ);

		int hashA = m_Permutation[floorX] + floorY;
		int hashAA = m_Permutation[hashA] + floorZ;
		int hashAB = m_Permutation[hashA + 1] + floorZ;
		int hashB = m_Permutation[floorX + 1] + floorY;
		int hashBA = m_Permutation[hashB] + floorZ;
		int hashBB = m_Permutation[hashB + 1] + floorZ;

		double gradAAA = Grad(m_Permutation[hashAA], fractionalX, fractionalY, fractionalZ);
		double gradBAA = Grad(m_Permutation[hashBA], fractionalX - 1.0, fractionalY, fractionalZ);
		double gradABA = Grad(m_Permutation[hashAB], fractionalX, fractionalY - 1.0, fractionalZ);
		double gradBBA = Grad(m_Permutation[hashBB], fractionalX - 1.0, fractionalY - 1.0, fractionalZ);
		double gradAAB = Grad(m_Permutation[hashAA + 1], fractionalX, fractionalY, fractionalZ - 1.0);
		double gradBAB = Grad(m_Permutation[hashBA + 1], fractionalX - 1.0, fractionalY, fractionalZ - 1.0);
		double gradABB = Grad(m_Permutation[hashAB + 1], fractionalX, fractionalY - 1.0, fractionalZ - 1.0);
		double gradBBB = Grad(m_Permutation[hashBB + 1], fractionalX - 1.0, fractionalY - 1.0, fractionalZ - 1.0);

		double lerpX1 = Lerp(fadeX, gradAAA, gradBAA);
		double lerpX2 = Lerp(fadeX, gradABA, gradBBA);
		double lerpX3 = Lerp(fadeX, gradAAB, gradBAB);
		double lerpX4 = Lerp(fadeX, gradABB, gradBBB);

		double lerpY1 = Lerp(fadeY, lerpX1, lerpX2);
		double lerpY2 = Lerp(fadeY, lerpX3, lerpX4);

		return Lerp(fadeZ, lerpY1, lerpY2);
	}

	double ClassicPerlinNoise::OctaveNoise2D(double x, double z, int octaves, double persistence) const {
		double total = 0.0;
		double frequency = 1.0;
		double amplitude = 1.0;
		double maxValue = 0.0;

		for (int octave = 0; octave < octaves; octave++) {
			total += Noise2D(x * frequency, z * frequency) * amplitude;
			maxValue += amplitude;
			amplitude *= persistence;
			frequency *= 2.0;
		}

		return total / maxValue;
	}

	double ClassicPerlinNoise::OctaveNoise3D(double x, double y, double z, int octaves, double persistence) const {
		double total = 0.0;
		double frequency = 1.0;
		double amplitude = 1.0;
		double maxValue = 0.0;

		for (int octave = 0; octave < octaves; octave++) {
			total += Noise3D(x * frequency, y * frequency, z * frequency) * amplitude;
			maxValue += amplitude;
			amplitude *= persistence;
			frequency *= 2.0;
		}

		return total / maxValue;
	}

	double ClassicPerlinNoise::Fade(double t) {
		return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
	}

	double ClassicPerlinNoise::Lerp(double t, double a, double b) {
		return a + t * (b - a);
	}

	double ClassicPerlinNoise::Grad(int hash, double x, double y, double z) {
		int bits = hash & 15;
		double u = bits < 8 ? x : y;
		double v = bits < 4 ? y : (bits == 12 || bits == 14 ? x : z);
		return ((bits & 1) ? -u : u) + ((bits & 2) ? -v : v);
	}

	double ClassicPerlinNoise::Grad2D(int hash, double x, double z) {
		switch (hash & 3) {
			case 0: return x + z;
			case 1: return -x + z;
			case 2: return x - z;
			case 3: return -x - z;
			default: return 0.0;
		}
	}

}
