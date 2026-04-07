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

		float Health() const { return m_Health; }
		float MaxHealth() const { return m_MaxHealth; }
		bool IsAlive() const { return m_Health > 0.0f; }

		void SetHealth(float health) {
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

		float AbsorptionAmount() const { return m_AbsorptionAmount; }
		void SetAbsorptionAmount(const float amount) { m_AbsorptionAmount = amount; }

		int ArrowCount() const { return m_ArrowCount; }
		void SetArrowCount(const int count) { m_ArrowCount = count; }

	protected:
		virtual void OnDeath() {}

		float m_Health = 20.0f;
		float m_MaxHealth = 20.0f;
		float m_AbsorptionAmount = 0.0f;
		int m_ArrowCount = 0;
	};

}
