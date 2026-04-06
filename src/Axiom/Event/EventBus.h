#pragma once

#include "Axiom/Event/Event.h"

#include <functional>
#include <string>
#include <vector>

namespace Axiom {

	enum class EventPriority {
		Lowest,
		Low,
		Normal,
		High,
		Highest,
		Monitor
	};

	class EventBus {
	public:
		using EventCallback = std::function<void(Event&)>;

		struct Subscription {
			EventType type;
			EventPriority priority;
			EventCallback callback;
			std::string owner;
		};

		void Subscribe(EventType type, EventCallback callback,
			EventPriority priority = EventPriority::Normal,
			const std::string& owner = "");

		void Publish(Event& event);

		void UnsubscribeAll(const std::string& owner);

	private:
		std::vector<Subscription> m_Subscriptions;
	};

}
