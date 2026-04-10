#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Event/Event.h"

#include <cstdint>
#include <string>
#include <utility>

namespace Axiom {

	enum class WeatherType;

	class LevelTimeChangedEvent : public Event {
	public:
		LevelTimeChangedEvent(const int64_t oldTime, const int64_t newTime)
			: m_OldTime(oldTime), m_NewTime(newTime) {}

		int64_t OldTime() const { return m_OldTime; }
		int64_t NewTime() const { return m_NewTime; }

		AX_EVENT_CLASS_TYPE(LevelTimeChanged)
		AX_EVENT_CLASS_CATEGORY(EventCategoryLevel)

	private:
		int64_t m_OldTime;
		int64_t m_NewTime;
	};

	class LevelWeatherChangedEvent : public Event {
	public:
		LevelWeatherChangedEvent(const WeatherType oldWeather, const WeatherType newWeather)
			: m_OldWeather(oldWeather), m_NewWeather(newWeather) {}

		WeatherType OldWeather() const { return m_OldWeather; }
		WeatherType NewWeather() const { return m_NewWeather; }

		AX_EVENT_CLASS_TYPE(LevelWeatherChanged)
		AX_EVENT_CLASS_CATEGORY(EventCategoryLevel)

	private:
		WeatherType m_OldWeather;
		WeatherType m_NewWeather;
	};

	class GameRuleChangedEvent : public Event {
	public:
		GameRuleChangedEvent(std::string ruleName, std::string oldValue, std::string newValue)
			: m_RuleName(std::move(ruleName))
			, m_OldValue(std::move(oldValue))
			, m_NewValue(std::move(newValue)) {}

		const std::string& RuleName() const { return m_RuleName; }
		const std::string& OldValue() const { return m_OldValue; }
		const std::string& NewValue() const { return m_NewValue; }

		AX_EVENT_CLASS_TYPE(GameRuleChanged)
		AX_EVENT_CLASS_CATEGORY(EventCategoryLevel)

	private:
		std::string m_RuleName;
		std::string m_OldValue;
		std::string m_NewValue;
	};

}
