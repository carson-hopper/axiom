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
		// Snapshot matching subscriptions then dispatch outside the
		// lock so callbacks can freely Subscribe / UnsubscribeAll /
		// Publish(nested) without self-deadlocking on m_Mutex.
		struct DispatchTarget {
			EventCallback callback;
			EventPriority priority;
		};

		std::vector<DispatchTarget> targets;
		{
			std::shared_lock const lock(m_Mutex);
			targets.reserve(m_Subscriptions.size());
			for (const auto& subscription : m_Subscriptions) {
				if (subscription.type == event.Type()) {
					targets.push_back({subscription.callback, subscription.priority});
				}
			}
		}

		for (const auto& target : targets) {
			if (event.m_Handled && target.priority != EventPriority::Monitor) {
				continue;
			}
			target.callback(event);
		}
	}

	void EventBus::UnsubscribeAll(const std::string& owner) {
		std::unique_lock lock(m_Mutex);

		std::erase_if(m_Subscriptions, [&owner](const Subscription& subscription) {
			return subscription.owner == owner;
		});
	}

}
