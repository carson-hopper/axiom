#pragma once

#include "Axiom/Core/Math.h"

namespace Axiom {

/**
 * Interface for controlling entity head/body rotation.
 * Goals write a target look position, and the control
 * smoothly rotates toward it each tick.
 */
class LookControl {
public:
	virtual ~LookControl() = default;

	/**
	 * Sets the world coordinates the entity should
	 * look toward.
	 */
	virtual void SetLookAt(double x, double y, double z) {
		m_LookX = x;
		m_LookY = y;
		m_LookZ = z;
		m_HasLookTarget = true;
	}

	/**
	 * Applies rotation toward the look target.
	 * Should be called once per server tick.
	 */
	virtual void Tick() {
		m_HasLookTarget = false;
	}

	bool HasLookTarget() const { return m_HasLookTarget; }
	double GetLookX() const { return m_LookX; }
	double GetLookY() const { return m_LookY; }
	double GetLookZ() const { return m_LookZ; }

protected:
	double m_LookX = 0.0;
	double m_LookY = 0.0;
	double m_LookZ = 0.0;
	bool m_HasLookTarget = false;
};

}
