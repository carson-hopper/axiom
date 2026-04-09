#pragma once

#include "Axiom/Core/Math.h"

namespace Axiom {

/**
 * Interface for controlling entity movement. A Mob owns
 * a MoveControl and goals write desired positions into
 * it. The control's Tick method translates those into
 * velocity changes each server tick.
 */
class MoveControl {
public:
	virtual ~MoveControl() = default;

	/**
	 * Sets the position the entity wants to reach
	 * and the speed multiplier to use.
	 */
	virtual void SetWantedPosition(double x, double y, double z,
		double speed) {
		m_WantedX = x;
		m_WantedY = y;
		m_WantedZ = z;
		m_SpeedModifier = speed;
		m_HasWanted = true;
	}

	/**
	 * Applies movement toward the wanted position.
	 * Should be called once per server tick.
	 */
	virtual void Tick() {
		m_HasWanted = false;
	}

	bool HasWantedPosition() const { return m_HasWanted; }
	double GetWantedX() const { return m_WantedX; }
	double GetWantedY() const { return m_WantedY; }
	double GetWantedZ() const { return m_WantedZ; }
	double GetSpeedModifier() const { return m_SpeedModifier; }

protected:
	double m_WantedX = 0.0;
	double m_WantedY = 0.0;
	double m_WantedZ = 0.0;
	double m_SpeedModifier = 1.0;
	bool m_HasWanted = false;
};

}
