#pragma once

namespace Axiom {

/**
 * Interface for controlling entity jumping. Goals
 * request a jump, and the control applies the impulse
 * on the next tick.
 */
class JumpControl {
public:
	virtual ~JumpControl() = default;

	/**
	 * Requests a jump on the next tick.
	 */
	virtual void Jump() {
		m_WantsJump = true;
	}

	/**
	 * Applies the jump impulse if requested and resets
	 * the flag. Should be called once per server tick.
	 */
	virtual void Tick() {
		m_WantsJump = false;
	}

	bool WantsToJump() const { return m_WantsJump; }

protected:
	bool m_WantsJump = false;
};

}
