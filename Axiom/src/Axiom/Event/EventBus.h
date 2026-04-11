#pragma once

#include "Axiom/Event/Event.h"

#include <functional>
#include <shared_mutex>
#include <string>
#include <vector>

namespace Axiom {

	enum class EventPriority : std::uint8_t {
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

		size_t SubscriptionCount() const {
			std::shared_lock lock(m_Mutex);
			return m_Subscriptions.size();
		}

	private:
		mutable std::shared_mutex m_Mutex;
		std::vector<Subscription> m_Subscriptions;
	};

}
