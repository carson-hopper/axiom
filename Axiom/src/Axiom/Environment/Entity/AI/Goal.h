#pragma once

#include "Axiom/Core/Base.h"

#include <cstdint>

namespace Axiom {

/**
 * Flags that describe the movement subsystems a goal
 * needs exclusive access to while running.
 */
enum class GoalFlag : uint8_t {
	Move   = 1 << 0,
	Look   = 1 << 1,
	Jump   = 1 << 2,
	Target = 1 << 3
};

inline uint8_t operator|(GoalFlag left, GoalFlag right) {
	return static_cast<uint8_t>(left) | static_cast<uint8_t>(right);
}

inline uint8_t operator|(uint8_t left, GoalFlag right) {
	return left | static_cast<uint8_t>(right);
}

inline bool HasFlag(uint8_t flags, GoalFlag flag) {
	return (flags & static_cast<uint8_t>(flag)) != 0;
}

/**
 * Abstract base class for mob AI goals.
 *
 * Each goal declares which subsystem flags it requires
 * and implements lifecycle hooks. The GoalSelector decides
 * which goals may run at any given tick based on priority
 * and flag conflicts.
 */
class Goal : public virtual RefCounted {
public:
	virtual ~Goal() = default;

	/**
	 * Returns true when this goal's preconditions are
	 * met and it should be considered for activation.
	 */
	virtual bool CanUse() = 0;

	/**
	 * Returns true when the goal is already running and
	 * its conditions still hold. Default returns CanUse().
	 */
	virtual bool CanContinueToUse() { return CanUse(); }

	/**
	 * Called once when the goal is first selected.
	 */
	virtual void Start() {}

	/**
	 * Called once when the goal is removed from the
	 * active set.
	 */
	virtual void Stop() {}

	/**
	 * Called every server tick while the goal is active.
	 */
	virtual void Tick() {}

	/**
	 * Returns the bitwise OR of GoalFlag values that
	 * this goal requires. Default is 0 (no flags).
	 */
	virtual uint8_t GetFlags() const { return 0; }
};

}
