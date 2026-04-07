#pragma once

#include "Axiom/Environment/Level/Generator/Noise.h"

#include <cmath>
#include <cstdint>
#include <vector>

namespace Axiom {

	/**
	 * Matches Minecraft's NormalNoise: multiple octaves of Perlin noise
	 * with configurable first octave and per-octave amplitudes.
	 *
	 * Vanilla uses this for all worldgen noises. The key insight is that
	 * firstOctave determines the base frequency: frequency = 2^(-firstOctave).
	 * Each subsequent octave doubles the frequency and halves the base
	 * amplitude, modulated by the amplitude array.
	 */
	class NormalNoise {
	public:
		struct Parameters {
			int firstOctave;
			std::vector<double> amplitudes;
		};

		NormalNoise(const uint64_t seed, const Parameters& parameters)
			: m_FirstPerlin(seed)
			, m_SecondPerlin(seed + 1) {

			const int firstOctave = parameters.firstOctave;
			const auto& amplitudes = parameters.amplitudes;
			const int octaveCount = static_cast<int>(amplitudes.size());

			// In vanilla, each octave has a frequency that doubles.
			// firstOctave = -9 means the first octave has period 2^9 = 512 blocks.
			// The Perlin noise is sampled at (coord / period), so higher octaves
			// sample at higher frequencies (smaller period = more detail).
			// Vanilla NormalNoise: each octave contributes amplitude / valueFactor
			// where valueFactor is the sum of all amplitudes (normalizes to ~[-1,1])
			double amplitudeSum = 0.0;

			for (int octave = 0; octave < octaveCount; octave++) {
				if (amplitudes[octave] != 0.0) {
					const double period = std::pow(2.0, -static_cast<double>(firstOctave + octave));
					m_Octaves.push_back({period, amplitudes[octave],
						seed + static_cast<uint64_t>(octave) * 2,
						seed + static_cast<uint64_t>(octave) * 2 + 1});
					amplitudeSum += amplitudes[octave];
				}
			}

			// Scale factor so that combined output is roughly in [-1, 1]
			// Vanilla uses 1/sum but Perlin output is ~[-0.7, 0.7] so we
			// use a less aggressive normalization to get full range
			m_ValueFactor = (amplitudeSum > 0) ? (1.0 / (amplitudeSum * 0.16)) : 1.0;
		}

		double GetValue(const double blockX, const double blockY, const double blockZ) const {
			double value = 0.0;

			for (const auto& octave : m_Octaves) {
				// Sample at (coord / period) — larger period = lower frequency
				const double sampleX = blockX / octave.period;
				const double sampleY = blockY / octave.period;
				const double sampleZ = blockZ / octave.period;

				const double firstSample = octave.firstNoise.Noise3D(sampleX, sampleY, sampleZ);
				const double secondSample = octave.secondNoise.Noise3D(
					sampleX + 100.0, sampleY + 100.0, sampleZ + 100.0);

				value += octave.amplitude * (firstSample + secondSample) * 0.5;
			}

			return value * m_ValueFactor;
		}

		/**
		 * 2D version (Y=0) for flat-cached noises like continentalness.
		 */
		double GetValue(const double blockX, const double blockZ) const {
			return GetValue(blockX, 0.0, blockZ);
		}

	private:
		struct OctaveData {
			double period;
			double amplitude;
			PerlinNoise firstNoise;
			PerlinNoise secondNoise;

			OctaveData(const double period, const double amplitude,
				const uint64_t seed1, const uint64_t seed2)
				: period(period)
				, amplitude(amplitude)
				, firstNoise(seed1)
				, secondNoise(seed2) {}
		};

		PerlinNoise m_FirstPerlin;
		PerlinNoise m_SecondPerlin;
		std::vector<OctaveData> m_Octaves;
		double m_ValueFactor = 0.0;
	};

	/**
	 * All vanilla 26.1 noise parameters extracted from the server.
	 */
	namespace NoiseParameters {

		inline NormalNoise::Parameters Continentalness() {
			return {-9, {1, 1, 2, 2, 2, 1, 1, 1, 1}};
		}

		inline NormalNoise::Parameters Erosion() {
			return {-9, {1, 1, 0, 1, 1}};
		}

		inline NormalNoise::Parameters Ridge() {
			return {-7, {1, 2, 1, 0, 0, 0}};
		}

		inline NormalNoise::Parameters Temperature() {
			return {-9, {1.5, 0, 1, 0, 0, 0}};
		}

		inline NormalNoise::Parameters Vegetation() {
			return {-8, {1, 1, 0, 0, 0, 0}};
		}

		inline NormalNoise::Parameters Offset() {
			return {-3, {1, 1, 1}};
		}

		inline NormalNoise::Parameters CaveCheese() {
			return {-8, {0.5, 1, 2, 1, 2, 1, 0, 2, 0}};
		}

		inline NormalNoise::Parameters CaveEntrance() {
			return {-7, {0.4, 0.5, 1.0}};
		}

		inline NormalNoise::Parameters CaveLayer() {
			return {-8, {1}};
		}

		inline NormalNoise::Parameters Spaghetti2D() {
			return {-1, {1}};
		}

		inline NormalNoise::Parameters Spaghetti3D1() {
			return {-1, {1}};
		}

		inline NormalNoise::Parameters Spaghetti3D2() {
			return {-1, {1}};
		}

		inline NormalNoise::Parameters SpaghettiRoughness() {
			return {-5, {1}};
		}

		inline NormalNoise::Parameters Noodle() {
			return {-8, {1}};
		}

		inline NormalNoise::Parameters NoodleThickness() {
			return {-8, {1}};
		}

		inline NormalNoise::Parameters NoodleRidgeA() {
			return {-7, {1}};
		}

		inline NormalNoise::Parameters NoodleRidgeB() {
			return {-7, {1}};
		}

		inline NormalNoise::Parameters AquiferBarrier() {
			return {-3, {1}};
		}

		inline NormalNoise::Parameters AquiferFloodedness() {
			return {-7, {1}};
		}

		inline NormalNoise::Parameters AquiferSpread() {
			return {-5, {1}};
		}

		inline NormalNoise::Parameters AquiferLava() {
			return {-1, {1}};
		}

		inline NormalNoise::Parameters ShiftX() {
			return {-6, {1, 1, 1, 1}};
		}

		inline NormalNoise::Parameters ShiftZ() {
			return {-6, {1, 1, 1, 1}};
		}

		inline NormalNoise::Parameters Surface() {
			return {-6, {1, 1, 1}};
		}

		inline NormalNoise::Parameters SurfaceSecondary() {
			return {-6, {1, 1, 0, 1}};
		}
	}

}
