#pragma once

#include "Axiom/Core/Base.h"

#include <string>

#define AX_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace Axiom {

	enum class EventType : std::uint8_t {
		None = 0,
		// Player
		PlayerJoin, PlayerQuit, PlayerChat, PlayerMove,
		PlayerHealthChanged, PlayerGameModeChanged, PlayerPositionChanged,
		PlayerHotbarChanged, PlayerFoodChanged, PlayerExperienceChanged,
		// Block
		BlockPlace, BlockBreak,
		// Server
		ServerStart, ServerStop, ServerTick,
		// Level
		LevelTimeChanged, LevelWeatherChanged,
		// GameRule
		GameRuleChanged,
		// Command
		CommandExecute,
		// Plugin
		PluginEnable, PluginDisable
	};

	enum EventCategory : std::uint8_t {
		EventCategoryNone    = 0,
		EventCategoryPlayer  = BIT(0),
		EventCategoryBlock   = BIT(1),
		EventCategoryServer  = BIT(2),
		EventCategoryCommand = BIT(3),
		EventCategoryPlugin  = BIT(4),
		EventCategoryLevel   = BIT(5),
	};

#define AX_EVENT_CLASS_TYPE(type) \
	static EventType StaticType() { return EventType::type; } \
	EventType Type() const override { return StaticType(); } \
	const std::string& Name() const override { return #type; }

#define AX_EVENT_CLASS_CATEGORY(category) \
	int CategoryFlags() const override { return category; }

	class Event {
	public:
		virtual ~Event() = default;

		virtual EventType Type() const = 0;
		virtual const std::string& Name() const = 0;
		virtual int CategoryFlags() const = 0;
		virtual std::string ToString() const { return Name(); }

		bool IsInCategory(EventCategory category) const {
			return CategoryFlags() & category;
		}

		bool m_Handled = false;
	};

}
