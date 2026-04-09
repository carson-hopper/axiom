#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Environment/Entity/AI/Goal.h"

#include <vector>

namespace Axiom {

/**
 * Manages a prioritized set of Goals and selects which
 * ones may run each tick.
 *
 * Lower priority values take precedence. When two goals
 * share at least one GoalFlag the higher-priority one wins
 * and the lower-priority one is stopped.
 */
class GoalSelector {
public:
	/**
	 * Adds a goal at the given priority. Lower values
	 * mean higher precedence.
	 */
	void AddGoal(int priority, Ref<Goal> goal);

	/**
	 * Removes a previously added goal.
	 */
	void RemoveGoal(const Ref<Goal>& goal);

	/**
	 * Evaluates all goals, starts eligible ones, stops
	 * conflicting or expired ones, and ticks the active
	 * set.
	 */
	void Tick();

	/**
	 * Stops all running goals.
	 */
	void StopAll();

private:
	struct PrioritizedGoal {
		int Priority;
		Ref<Goal> GoalInstance;
		bool Running = false;
	};

	/**
	 * Returns true when the given flags conflict with
	 * any goal that is running at a higher (lower
	 * numeric) priority.
	 */
	bool HasConflict(const PrioritizedGoal& candidate) const;

	std::vector<PrioritizedGoal> m_Goals;
};

}
