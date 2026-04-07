#pragma once

#include "Axiom/Environment/World/Generator/Noise.h"

#include <algorithm>
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

			// Calculate frequency and value factor matching vanilla
			m_ValueFactor = 0.0;
			double frequency = std::pow(2.0, static_cast<double>(firstOctave));

			for (int octave = 0; octave < octaveCount; octave++) {
				if (amplitudes[octave] != 0.0) {
					m_Octaves.push_back({frequency, amplitudes[octave], seed + octave * 2, seed + octave * 2 + 1});
					m_ValueFactor += amplitudes[octave] / frequency;
				}
				frequency *= 2.0;
			}

			// Normalize so output is roughly in [-1, 1]
			if (m_ValueFactor > 0) {
				m_ValueFactor = 1.0 / m_ValueFactor;
			}
		}

		double GetValue(const double blockX, const double blockY, const double blockZ) const {
			double value = 0.0;

			for (const auto& octave : m_Octaves) {
				// Vanilla samples two independent Perlin noises and averages them
				const double firstSample = octave.firstNoise.Noise3D(
					blockX / octave.frequency,
					blockY / octave.frequency,
					blockZ / octave.frequency);
				const double secondSample = octave.secondNoise.Noise3D(
					blockX / octave.frequency,
					blockY / octave.frequency,
					blockZ / octave.frequency);

				value += octave.amplitude * (firstSample + secondSample) * 0.5 / octave.frequency;
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
			double frequency;
			double amplitude;
			PerlinNoise firstNoise;
			PerlinNoise secondNoise;

			OctaveData(const double frequency, const double amplitude,
				const uint64_t seed1, const uint64_t seed2)
				: frequency(frequency)
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
