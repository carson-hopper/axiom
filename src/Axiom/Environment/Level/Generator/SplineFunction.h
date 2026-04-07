#pragma once

#include <algorithm>
#include <vector>

namespace Axiom {

	/**
	 * Cubic Hermite spline matching Minecraft's terrain height splines.
	 *
	 * Maps an input parameter (like continentalness) to an output value
	 * (like base terrain height) using smooth interpolation between
	 * control points.
	 */
	class SplineFunction {
	public:
		struct Point {
			double location;    // Input parameter value
			double value;       // Output value at this point
			double derivative;  // Slope at this point
		};

		SplineFunction() = default;

		explicit SplineFunction(std::vector<Point> points)
			: m_Points(std::move(points)) {}

		/**
		 * Evaluate the spline at the given input value.
		 */
		double Apply(const double input) const {
			if (m_Points.empty()) return 0.0;
			if (m_Points.size() == 1) return m_Points[0].value;

			// Clamp to spline range
			if (input <= m_Points.front().location) return m_Points.front().value;
			if (input >= m_Points.back().location) return m_Points.back().value;

			// Find the segment containing the input
			int segmentIndex = 0;
			for (int index = 0; index < static_cast<int>(m_Points.size()) - 1; index++) {
				if (input < m_Points[index + 1].location) {
					segmentIndex = index;
					break;
				}
			}

			const Point& pointA = m_Points[segmentIndex];
			const Point& pointB = m_Points[segmentIndex + 1];

			// Normalize t to [0, 1] within this segment
			const double segmentLength = pointB.location - pointA.location;
			const double normalizedPosition = (input - pointA.location) / segmentLength;

			// Cubic Hermite interpolation
			return CubicHermite(
				pointA.value, pointB.value,
				pointA.derivative * segmentLength,
				pointB.derivative * segmentLength,
				normalizedPosition);
		}

		/**
		 * Build a terrain offset spline matching vanilla's continentalness → height mapping.
		 * This is the core of what makes MC terrain look like MC terrain.
		 */
		static SplineFunction BuildTerrainOffset() {
			return SplineFunction({
				{-1.10, -0.100, 0.00},   // Deep ocean floor
				{-1.02, -0.050, 0.00},   // Ocean
				{-0.51, -0.050, 0.00},   // Shallow ocean
				{-0.44, -0.020, 0.00},   // Coast
				{-0.18, -0.020, 0.00},   // Beach
				{-0.16,  0.000, 0.00},   // Shore
				{-0.15,  0.005, 0.10},   // Near coast land
				{-0.10,  0.010, 0.30},   // Low land
				{ 0.25,  0.060, 0.20},   // Plains/forest
				{ 0.50,  0.100, 0.50},   // Hills
				{ 1.00,  0.450, 1.50},   // Mountains
				{ 1.50,  0.800, 0.00},   // Extreme mountains
			});
		}

		/**
		 * Erosion factor spline: how much erosion flattens terrain.
		 */
		static SplineFunction BuildErosionFactor() {
			return SplineFunction({
				{-1.0,  1.00, 0.0},   // No erosion = full terrain amplitude
				{-0.5,  0.80, 0.0},
				{ 0.0,  0.60, 0.0},   // Moderate erosion
				{ 0.5,  0.30, 0.0},   // Heavy erosion = flatter
				{ 1.0,  0.10, 0.0},   // Maximum erosion = very flat
			});
		}

		/**
		 * Ridge factor: converts raw ridge noise to peaks & valleys parameter.
		 * Vanilla uses abs(abs(noise) - 2/3) - 1/3 then spline.
		 */
		static SplineFunction BuildRidgeFactor() {
			return SplineFunction({
				{-1.0,  0.05, 0.0},   // Valley floor
				{-0.7, -0.10, 0.0},   // Valley edge
				{-0.4, -0.10, 0.0},   // Flat area
				{ 0.0,  0.00, 0.0},   // Neutral
				{ 0.4,  0.10, 0.0},   // Ridge base
				{ 0.7,  0.30, 0.5},   // Ridge
				{ 1.0,  0.50, 0.0},   // Peak
			});
		}

		/**
		 * Jaggedness spline: how rough/jagged mountain peaks are.
		 */
		static SplineFunction BuildJaggedness() {
			return SplineFunction({
				{-1.0,  0.00, 0.0},
				{-0.5,  0.00, 0.0},
				{ 0.0,  0.00, 0.0},
				{ 0.4,  0.00, 0.0},
				{ 0.5,  0.10, 0.5},
				{ 0.7,  0.40, 1.0},
				{ 1.0,  0.80, 0.0},
			});
		}

	private:
		static double CubicHermite(const double valueA, const double valueB,
			const double tangentA, const double tangentB, const double normalizedPosition) {

			const double positionSquared = normalizedPosition * normalizedPosition;
			const double positionCubed = positionSquared * normalizedPosition;

			return (2.0 * positionCubed - 3.0 * positionSquared + 1.0) * valueA
				+ (positionCubed - 2.0 * positionSquared + normalizedPosition) * tangentA
				+ (-2.0 * positionCubed + 3.0 * positionSquared) * valueB
				+ (positionCubed - positionSquared) * tangentB;
		}

		std::vector<Point> m_Points;
	};

}
