#pragma once

#include "Axiom/Core/Math.h"

#include <algorithm>

namespace Axiom {

	/**
	 * Axis-aligned bounding box defined by
	 * minimum and maximum corner points.
	 * Used for collision detection, entity
	 * bounds, and spatial queries.
	 */
	class AABB {
	public:
		AABB() = default;

		AABB(double minX, double minY, double minZ,
			double maxX, double maxY, double maxZ)
			: m_MinX(minX), m_MinY(minY), m_MinZ(minZ)
			, m_MaxX(maxX), m_MaxY(maxY), m_MaxZ(maxZ) {}

		AABB(const Vector3& minimum, const Vector3& maximum)
			: m_MinX(minimum.x), m_MinY(minimum.y), m_MinZ(minimum.z)
			, m_MaxX(maximum.x), m_MaxY(maximum.y), m_MaxZ(maximum.z) {}

		// ----- Accessors ------------------------------------------------

		double MinX() const { return m_MinX; }
		double MinY() const { return m_MinY; }
		double MinZ() const { return m_MinZ; }
		double MaxX() const { return m_MaxX; }
		double MaxY() const { return m_MaxY; }
		double MaxZ() const { return m_MaxZ; }

		Vector3 Min() const { return {m_MinX, m_MinY, m_MinZ}; }
		Vector3 Max() const { return {m_MaxX, m_MaxY, m_MaxZ}; }
		Vector3 Center() const {
			return {
				(m_MinX + m_MaxX) * 0.5,
				(m_MinY + m_MaxY) * 0.5,
				(m_MinZ + m_MaxZ) * 0.5
			};
		}

		double Width() const { return m_MaxX - m_MinX; }
		double Height() const { return m_MaxY - m_MinY; }
		double Depth() const { return m_MaxZ - m_MinZ; }

		// ----- Tests ----------------------------------------------------

		/**
		 * Whether a point lies inside
		 * this bounding box.
		 */
		bool Contains(double pointX, double pointY, double pointZ) const {
			return pointX >= m_MinX && pointX <= m_MaxX
				&& pointY >= m_MinY && pointY <= m_MaxY
				&& pointZ >= m_MinZ && pointZ <= m_MaxZ;
		}

		bool Contains(const Vector3& point) const {
			return Contains(point.x, point.y, point.z);
		}

		/**
		 * Whether this box overlaps
		 * another box.
		 */
		bool Intersects(const AABB& other) const {
			return m_MinX < other.m_MaxX && m_MaxX > other.m_MinX
				&& m_MinY < other.m_MaxY && m_MaxY > other.m_MinY
				&& m_MinZ < other.m_MaxZ && m_MaxZ > other.m_MinZ;
		}

		// ----- Transformations ------------------------------------------

		/**
		 * Return a new AABB expanded by
		 * the given amount in all six
		 * directions.
		 */
		AABB Expand(double amount) const {
			return {
				m_MinX - amount, m_MinY - amount, m_MinZ - amount,
				m_MaxX + amount, m_MaxY + amount, m_MaxZ + amount
			};
		}

		/**
		 * Return a new AABB translated
		 * by the given delta.
		 */
		AABB Move(double deltaX, double deltaY, double deltaZ) const {
			return {
				m_MinX + deltaX, m_MinY + deltaY, m_MinZ + deltaZ,
				m_MaxX + deltaX, m_MaxY + deltaY, m_MaxZ + deltaZ
			};
		}

		AABB Move(const Vector3& delta) const {
			return Move(delta.x, delta.y, delta.z);
		}

		/**
		 * Return the smallest AABB that
		 * contains both this box and
		 * another box.
		 */
		AABB Merge(const AABB& other) const {
			return {
				std::min(m_MinX, other.m_MinX),
				std::min(m_MinY, other.m_MinY),
				std::min(m_MinZ, other.m_MinZ),
				std::max(m_MaxX, other.m_MaxX),
				std::max(m_MaxY, other.m_MaxY),
				std::max(m_MaxZ, other.m_MaxZ)
			};
		}

		// ----- Axis clip ------------------------------------------------

		/**
		 * Clip movement along the X axis
		 * against another AABB, returning
		 * the allowed delta.
		 */
		double ClipXCollide(const AABB& other, double deltaX) const;

		/**
		 * Clip movement along the Y axis
		 * against another AABB, returning
		 * the allowed delta.
		 */
		double ClipYCollide(const AABB& other, double deltaY) const;

		/**
		 * Clip movement along the Z axis
		 * against another AABB, returning
		 * the allowed delta.
		 */
		double ClipZCollide(const AABB& other, double deltaZ) const;

		// ----- Factory --------------------------------------------------

		/**
		 * Create an AABB centered on a
		 * position with the given half-
		 * widths.
		 */
		static AABB FromCenterAndSize(const Vector3& center, double halfWidth, double halfHeight) {
			return {
				center.x - halfWidth, center.y, center.z - halfWidth,
				center.x + halfWidth, center.y + halfHeight * 2.0, center.z + halfWidth
			};
		}

		/**
		 * Create a unit-cube AABB for a
		 * block at integer coordinates.
		 */
		static AABB BlockAABB(int blockX, int blockY, int blockZ) {
			return {
				static_cast<double>(blockX),
				static_cast<double>(blockY),
				static_cast<double>(blockZ),
				static_cast<double>(blockX) + 1.0,
				static_cast<double>(blockY) + 1.0,
				static_cast<double>(blockZ) + 1.0
			};
		}

	private:
		double m_MinX = 0.0;
		double m_MinY = 0.0;
		double m_MinZ = 0.0;
		double m_MaxX = 0.0;
		double m_MaxY = 0.0;
		double m_MaxZ = 0.0;
	};

}
