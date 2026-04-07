#pragma once

#include <cstdint>
#include <functional>

namespace Axiom {

	/**
	 * Samples a density function at the corners of 4×8×4 block cells
	 * and trilinearly interpolates within each cell.
	 *
	 * Vanilla uses this as the primary performance optimization:
	 * instead of sampling noise at every block (4096 per section),
	 * it samples at cell corners (~125 per section) and interpolates.
	 *
	 * Cell grid for one chunk (16×384×16 blocks):
	 *   X: 4 cells of 4 blocks = 16 blocks
	 *   Y: 48 cells of 8 blocks = 384 blocks
	 *   Z: 4 cells of 4 blocks = 16 blocks
	 *   = 5×49×5 = 1225 corner samples per chunk (vs 98,304 blocks)
	 */
	class CellInterpolator {
	public:
		static constexpr int CellWidth = 4;
		static constexpr int CellHeight = 8;
		static constexpr int CellsX = 4;    // 16 / 4
		static constexpr int CellsY = 48;   // 384 / 8
		static constexpr int CellsZ = 4;    // 16 / 4
		static constexpr int CornersX = CellsX + 1;  // 5
		static constexpr int CornersY = CellsY + 1;  // 49
		static constexpr int CornersZ = CellsZ + 1;  // 5
		static constexpr int MinY = -64;

		using DensitySampler = std::function<double(int, int, int)>;

		/**
		 * Sample density at all cell corners for this chunk.
		 */
		void SampleCorners(const int32_t chunkX, const int32_t chunkZ,
			const DensitySampler& sampler) {

			const int baseBlockX = chunkX * 16;
			const int baseBlockZ = chunkZ * 16;

			for (int cornerZ = 0; cornerZ < CornersZ; cornerZ++) {
				for (int cornerX = 0; cornerX < CornersX; cornerX++) {
					for (int cornerY = 0; cornerY < CornersY; cornerY++) {
						const int worldX = baseBlockX + cornerX * CellWidth;
						const int worldY = MinY + cornerY * CellHeight;
						const int worldZ = baseBlockZ + cornerZ * CellWidth;

						m_Corners[cornerZ][cornerX][cornerY] = sampler(worldX, worldY, worldZ);
					}
				}
			}
		}

		/**
		 * Get interpolated density at any block position within the chunk.
		 * Uses trilinear interpolation between the 8 surrounding cell corners.
		 */
		double GetDensity(const int localX, const int worldY, const int localZ) const {
			// Which cell are we in?
			const int cellX = localX / CellWidth;
			const int cellZ = localZ / CellWidth;
			const int cellY = (worldY - MinY) / CellHeight;

			// Clamp to valid cell range
			const int safeX = std::min(cellX, CellsX - 1);
			const int safeZ = std::min(cellZ, CellsZ - 1);
			const int safeY = std::clamp(cellY, 0, CellsY - 1);

			// Fractional position within the cell [0, 1)
			const double fractionX = static_cast<double>(localX % CellWidth) / CellWidth;
			const double fractionZ = static_cast<double>(localZ % CellWidth) / CellWidth;
			const double fractionY = static_cast<double>((worldY - MinY) % CellHeight) / CellHeight;

			// 8 corner values
			const double corner000 = m_Corners[safeZ][safeX][safeY];
			const double corner100 = m_Corners[safeZ][safeX + 1][safeY];
			const double corner010 = m_Corners[safeZ][safeX][safeY + 1];
			const double corner110 = m_Corners[safeZ][safeX + 1][safeY + 1];
			const double corner001 = m_Corners[safeZ + 1][safeX][safeY];
			const double corner101 = m_Corners[safeZ + 1][safeX + 1][safeY];
			const double corner011 = m_Corners[safeZ + 1][safeX][safeY + 1];
			const double corner111 = m_Corners[safeZ + 1][safeX + 1][safeY + 1];

			// Trilinear interpolation
			return Lerp(
				Lerp(
					Lerp(corner000, corner100, fractionX),
					Lerp(corner010, corner110, fractionX),
					fractionY),
				Lerp(
					Lerp(corner001, corner101, fractionX),
					Lerp(corner011, corner111, fractionX),
					fractionY),
				fractionZ);
		}

	private:
		static double Lerp(const double valueA, const double valueB, const double fraction) {
			return valueA + fraction * (valueB - valueA);
		}

		// [z_corner][x_corner][y_corner]
		double m_Corners[CornersZ][CornersX][CornersY]{};
	};

}
