#include "Axiom/Environment/Entity/AI/GoalSelector.h"

#include <algorithm>

namespace Axiom {

void GoalSelector::AddGoal(int priority, Ref<Goal> goal) {
	m_Goals.push_back({priority, std::move(goal), false});
	std::sort(m_Goals.begin(), m_Goals.end(),
		[](const PrioritizedGoal& left, const PrioritizedGoal& right) {
			return left.Priority < right.Priority;
		});
}

void GoalSelector::RemoveGoal(const Ref<Goal>& goal) {
	for (auto iterator = m_Goals.begin(); iterator != m_Goals.end(); ++iterator) {
		if (iterator->GoalInstance == goal) {
			if (iterator->Running) {
				iterator->GoalInstance->Stop();
			}
			m_Goals.erase(iterator);
			return;
		}
	}
}

void GoalSelector::Tick() {
	// Phase 1: stop goals that can no longer continue
	for (auto& entry : m_Goals) {
		if (entry.Running && !entry.GoalInstance->CanContinueToUse()) {
			entry.GoalInstance->Stop();
			entry.Running = false;
		}
	}

	// Phase 2: try to start new goals (already sorted by priority)
	for (auto& entry : m_Goals) {
		if (entry.Running) {
			continue;
		}
		if (!entry.GoalInstance->CanUse()) {
			continue;
		}
		if (HasConflict(entry)) {
			continue;
		}

		// Stop lower-priority goals that share flags
		uint8_t candidateFlags = entry.GoalInstance->GetFlags();
		if (candidateFlags != 0) {
			for (auto& other : m_Goals) {
				if (&other == &entry) {
					continue;
				}
				if (!other.Running) {
					continue;
				}
				if (other.Priority > entry.Priority &&
					(other.GoalInstance->GetFlags() & candidateFlags) != 0) {
					other.GoalInstance->Stop();
					other.Running = false;
				}
			}
		}

		entry.GoalInstance->Start();
		entry.Running = true;
	}

	// Phase 3: tick all running goals
	for (auto& entry : m_Goals) {
		if (entry.Running) {
			entry.GoalInstance->Tick();
		}
	}
}

void GoalSelector::StopAll() {
	for (auto& entry : m_Goals) {
		if (entry.Running) {
			entry.GoalInstance->Stop();
			entry.Running = false;
		}
	}
}

bool GoalSelector::HasConflict(const PrioritizedGoal& candidate) const {
	uint8_t candidateFlags = candidate.GoalInstance->GetFlags();
	if (candidateFlags == 0) {
		return false;
	}

	for (auto& other : m_Goals) {
		if (&other == &candidate) {
			continue;
		}
		if (!other.Running) {
			continue;
		}
		// A running goal with higher priority (lower number)
		// that shares flags blocks this candidate
		if (other.Priority < candidate.Priority &&
			(other.GoalInstance->GetFlags() & candidateFlags) != 0) {
			return true;
		}
	}
	return false;
}

}
