#include "AABB.h"

#include <cmath>

namespace Axiom {

	double AABB::ClipXCollide(const AABB& other, double deltaX) const {
		// Y and Z axes must overlap for an X collision
		if (other.m_MaxY <= m_MinY || other.m_MinY >= m_MaxY) return deltaX;
		if (other.m_MaxZ <= m_MinZ || other.m_MinZ >= m_MaxZ) return deltaX;

		if (deltaX > 0.0 && other.m_MaxX <= m_MinX) {
			double clip = m_MinX - other.m_MaxX;
			if (clip < deltaX) {
				deltaX = clip;
			}
		} else if (deltaX < 0.0 && other.m_MinX >= m_MaxX) {
			double clip = m_MaxX - other.m_MinX;
			if (clip > deltaX) {
				deltaX = clip;
			}
		}

		return deltaX;
	}

	double AABB::ClipYCollide(const AABB& other, double deltaY) const {
		// X and Z axes must overlap for a Y collision
		if (other.m_MaxX <= m_MinX || other.m_MinX >= m_MaxX) return deltaY;
		if (other.m_MaxZ <= m_MinZ || other.m_MinZ >= m_MaxZ) return deltaY;

		if (deltaY > 0.0 && other.m_MaxY <= m_MinY) {
			double clip = m_MinY - other.m_MaxY;
			if (clip < deltaY) {
				deltaY = clip;
			}
		} else if (deltaY < 0.0 && other.m_MinY >= m_MaxY) {
			double clip = m_MaxY - other.m_MinY;
			if (clip > deltaY) {
				deltaY = clip;
			}
		}

		return deltaY;
	}

	double AABB::ClipZCollide(const AABB& other, double deltaZ) const {
		// X and Y axes must overlap for a Z collision
		if (other.m_MaxX <= m_MinX || other.m_MinX >= m_MaxX) return deltaZ;
		if (other.m_MaxY <= m_MinY || other.m_MinY >= m_MaxY) return deltaZ;

		if (deltaZ > 0.0 && other.m_MaxZ <= m_MinZ) {
			double clip = m_MinZ - other.m_MaxZ;
			if (clip < deltaZ) {
				deltaZ = clip;
			}
		} else if (deltaZ < 0.0 && other.m_MinZ >= m_MaxZ) {
			double clip = m_MaxZ - other.m_MinZ;
			if (clip > deltaZ) {
				deltaZ = clip;
			}
		}

		return deltaZ;
	}

}
