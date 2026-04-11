#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Core/Math.h"
#include "Axiom/Event/Event.h"

#include <string>

namespace Axiom {

	class Player;

	enum class GameMode;
	enum class OpLevel;

	/**
	 * Base class for all player property-change events.
	 * Holds a Ref<Player> so the event remains valid
	 * for the duration of dispatch even if the player
	 * disconnects mid-publish. Player is forward-declared
	 * above to avoid pulling Player.h into every consumer.
	 */
	class PlayerPropertyEvent : public Event {
	public:
		explicit PlayerPropertyEvent(const Ref<Player>& player) : m_Player(player) {}

		Ref<Player> GetPlayer() const { return m_Player; }

		AX_EVENT_CLASS_CATEGORY(EventCategoryPlayer)

	private:
		Ref<Player> m_Player;
	};

	class PlayerHealthChangedEvent : public PlayerPropertyEvent {
	public:
		PlayerHealthChangedEvent(const Ref<Player>& player, const int oldHealth, const int newHealth)
			: PlayerPropertyEvent(player), m_OldHealth(oldHealth), m_NewHealth(newHealth) {}

		int OldHealth() const { return m_OldHealth; }
		int NewHealth() const { return m_NewHealth; }

		AX_EVENT_CLASS_TYPE(PlayerHealthChanged)

	private:
		int m_OldHealth;
		int m_NewHealth;
	};

	class PlayerGameModeChangedEvent : public PlayerPropertyEvent {
	public:
		PlayerGameModeChangedEvent(const Ref<Player>& player, const GameMode oldMode, const GameMode newMode)
			: PlayerPropertyEvent(player), m_OldMode(oldMode), m_NewMode(newMode) {}

		GameMode OldMode() const { return m_OldMode; }
		GameMode NewMode() const { return m_NewMode; }

		AX_EVENT_CLASS_TYPE(PlayerGameModeChanged)

	private:
		GameMode m_OldMode;
		GameMode m_NewMode;
	};

	class PlayerPositionChangedEvent : public PlayerPropertyEvent {
	public:
		PlayerPositionChangedEvent(const Ref<Player>& player, const Vector3& oldPos, const Vector3& newPos)
			: PlayerPropertyEvent(player), m_OldPosition(oldPos), m_NewPosition(newPos) {}

		const Vector3& OldPosition() const { return m_OldPosition; }
		const Vector3& NewPosition() const { return m_NewPosition; }

		AX_EVENT_CLASS_TYPE(PlayerPositionChanged)

	private:
		Vector3 m_OldPosition;
		Vector3 m_NewPosition;
	};

	class PlayerHotbarChangedEvent : public PlayerPropertyEvent {
	public:
		PlayerHotbarChangedEvent(const Ref<Player>& player, const int oldSlot, const int newSlot)
			: PlayerPropertyEvent(player), m_OldSlot(oldSlot), m_NewSlot(newSlot) {}

		int OldSlot() const { return m_OldSlot; }
		int NewSlot() const { return m_NewSlot; }

		AX_EVENT_CLASS_TYPE(PlayerHotbarChanged)

	private:
		int m_OldSlot;
		int m_NewSlot;
	};

	class PlayerFoodChangedEvent : public PlayerPropertyEvent {
	public:
		PlayerFoodChangedEvent(const Ref<Player>& player, const int oldFood, const int newFood)
			: PlayerPropertyEvent(player), m_OldFood(oldFood), m_NewFood(newFood) {}

		int OldFood() const { return m_OldFood; }
		int NewFood() const { return m_NewFood; }

		AX_EVENT_CLASS_TYPE(PlayerFoodChanged)

	private:
		int m_OldFood;
		int m_NewFood;
	};

	class PlayerExperienceChangedEvent : public PlayerPropertyEvent {
	public:
		PlayerExperienceChangedEvent(const Ref<Player>& player, const int oldLevel, const int newLevel)
			: PlayerPropertyEvent(player), m_OldLevel(oldLevel), m_NewLevel(newLevel) {}

		int OldLevel() const { return m_OldLevel; }
		int NewLevel() const { return m_NewLevel; }

		AX_EVENT_CLASS_TYPE(PlayerExperienceChanged)

	private:
		int m_OldLevel;
		int m_NewLevel;
	};

	class PlayerOperatorLevelChangeEvent : public PlayerPropertyEvent {
	public:
		PlayerOperatorLevelChangeEvent(const Ref<Player>& player, const OpLevel oldLevel, const OpLevel newLevel)
			: PlayerPropertyEvent(player), m_OldLevel(oldLevel), m_NewLevel(newLevel) {}

		OpLevel OldLevel() const { return m_OldLevel; }
		OpLevel NewLevel() const { return m_NewLevel; }

		AX_EVENT_CLASS_TYPE(PlayerOperatorLevelChanged)

	private:
		OpLevel m_OldLevel;
		OpLevel m_NewLevel;
	};

}
