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
		// Snapshot-then-dispatch. We MUST NOT call user
		// callbacks while holding `m_Mutex` because they
		// are free to re-enter the bus (Subscribe a new
		// handler, UnsubscribeAll for their plugin, or
		// Publish a nested event). `std::shared_mutex`
		// is not reentrant — any of those would either
		// deadlock on self (unique_lock + held shared),
		// deadlock against a waiting writer (recursive
		// shared_lock under writer preference), or
		// invalidate the iterator we're walking.
		//
		// Instead: copy matching subscriptions into a
		// local vector under a shared_lock, release the
		// lock, then dispatch from the local copy. A
		// callback that unsubscribes itself still fires
		// once on THIS dispatch (matching Bukkit/Spigot
		// semantics) but is gone for the next one.
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

		// Dispatch without holding the lock. Callbacks
		// are free to mutate the bus — the mutations
		// land in `m_Subscriptions` and take effect on
		// the next publish.
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
