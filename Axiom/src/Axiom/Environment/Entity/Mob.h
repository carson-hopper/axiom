#pragma once

#include "Axiom/Environment/Entity/LivingEntity.h"

namespace Axiom {

	/**
	 * Base class for AI-driven entities (zombies,
	 * skeletons, animals, etc.) that extend
	 * LivingEntity with mob-specific
	 * behaviour hooks.
	 */
	class Mob : public LivingEntity {
	public:
		explicit Mob(int32_t entityId);

		/** Goal selector stub -- will be filled in Task 25. */
		// GoalSelector& Goals() { return m_GoalSelector; }

		/** Navigation stub. */
		bool HasNavigation() const { return false; }

		/** Whether this mob is persistent (won't despawn). */
		bool IsPersistent() const { return m_Persistent; }
		void SetPersistent(bool persistent) { m_Persistent = persistent; }

		/** Whether this mob can pick up items. */
		bool CanPickUpLoot() const { return m_CanPickUpLoot; }
		void SetCanPickUpLoot(bool canPickUp) { m_CanPickUpLoot = canPickUp; }

		/** No-persistence distance check for despawning. */
		float DespawnDistance() const { return m_DespawnDistance; }
		void SetDespawnDistance(float distance) { m_DespawnDistance = distance; }

		/** Whether this mob is aggressive. */
		bool IsAggressive() const { return m_Aggressive; }
		void SetAggressive(bool aggressive) { m_Aggressive = aggressive; }

	private:
		bool m_Persistent = false;
		bool m_CanPickUpLoot = false;
		bool m_Aggressive = false;
		float m_DespawnDistance = 128.0f;
	};

}
