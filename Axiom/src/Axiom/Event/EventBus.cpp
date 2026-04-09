#include "EventBus.h"

#include "Axiom/Core/Log.h"

#include <algorithm>

namespace Axiom {

	void EventBus::Subscribe(EventType type, EventCallback callback,
		EventPriority priority, const std::string& owner) {

		std::unique_lock lock(m_Mutex);

		Subscription subscription{type, priority, std::move(callback), owner};

		auto insertPosition = std::upper_bound(
			m_Subscriptions.begin(), m_Subscriptions.end(), subscription,
			[](const Subscription& left, const Subscription& right) {
				return left.priority > right.priority;
			});

		m_Subscriptions.insert(insertPosition, std::move(subscription));
	}

	void EventBus::Publish(Event& event) {
		std::shared_lock lock(m_Mutex);

		for (auto& subscription : m_Subscriptions) {
			if (subscription.type != event.Type()) {
				continue;
			}

			if (event.m_Handled && subscription.priority != EventPriority::Monitor) {
				continue;
			}

			subscription.callback(event);
		}
	}

	void EventBus::UnsubscribeAll(const std::string& owner) {
		std::unique_lock lock(m_Mutex);

		std::erase_if(m_Subscriptions, [&owner](const Subscription& subscription) {
			return subscription.owner == owner;
		});
	}

}
