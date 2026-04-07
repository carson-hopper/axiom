#pragma once

#include "Axiom/Environment/Entity/Entity.h"

namespace Axiom {

	/**
	 * Base class for entities that have health, can take damage, and die.
	 * Players, mobs, and animals all extend LivingEntity.
	 */
	class LivingEntity : public Entity {
	public:
		explicit LivingEntity(const int32_t entityId)
			: Entity(entityId) {}

		float GetHealth() const { return m_Health; }
		float GetMaxHealth() const { return m_MaxHealth; }
		bool IsAlive() const { return m_Health > 0.0f; }

		void SetHealth(const float health) {
			m_Health = std::min(health, m_MaxHealth);
			if (m_Health <= 0.0f) {
				m_Health = 0.0f;
				OnDeath();
			}
		}

		void SetMaxHealth(const float maxHealth) { m_MaxHealth = maxHealth; }

		void Damage(const float amount) {
			if (!IsAlive()) return;
			SetHealth(m_Health - amount);
		}

		void Heal(const float amount) {
			if (!IsAlive()) return;
			SetHealth(m_Health + amount);
		}

	protected:
		virtual void OnDeath() {}

		float m_Health = 20.0f;
		float m_MaxHealth = 20.0f;
	};

}
