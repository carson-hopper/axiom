#pragma once

#include "Axiom/Event/Event.h"

#include <functional>
#include <shared_mutex>
#include <string>
#include <vector>

namespace Axiom {

	/**
	 * Event priority levels control the order in which handlers are invoked.
	 *
	 * Events are dispatched from Lowest to Highest priority. Monitor priority
	 * handlers are always called last, even if the event was cancelled.
	 */
	enum class EventPriority : std::uint8_t {
		Lowest,   ///< Called first; good for early filtering/preprocessing
		Low,      ///< Early handlers
		Normal,   ///< Default priority for most handlers
		High,     ///< Late handlers that need to see filtered data
		Highest,  ///< Called last before monitor; good for final validation
		Monitor   ///< Always called; use only for logging/metrics (read-only!)
	};

	/**
	 * Thread-safe event publish/subscribe system.
	 *
	 * The EventBus allows decoupled communication between components. Publishers
	 * send events without knowing about subscribers; subscribers register callbacks
	 * for specific event types.
	 *
	 * Thread safety:
	 * - Subscribe / UnsubscribeAll: Thread-safe (acquires unique_lock)
	 * - Publish: Thread-safe (acquires shared_lock ONLY while snapshotting
	 *   matching subscriptions; dispatch runs after the lock is released)
	 * - Callbacks are invoked synchronously on the caller's thread
	 *
	 * Reentrancy:
	 * - Callbacks MAY call `Subscribe`, `UnsubscribeAll`, or `Publish` for
	 *   nested events without deadlocking. These operations run outside the
	 *   publish lock thanks to the snapshot-then-dispatch strategy.
	 * - A subscription added inside a callback takes effect on the NEXT
	 *   publish.
	 * - A subscription removed inside a callback still fires once on the
	 *   current publish (matching Bukkit/Spigot semantics).
	 *
	 * Performance note: Subscriptions are kept sorted by priority. Insertion
	 * is O(n) but publishing is O(m) where m = matching subscriptions. Each
	 * `Publish` copies its matching subscriptions into a local vector so the
	 * dispatch is lock-free with respect to the bus itself.
	 *
	 * Example usage:
	 * @code
	 * EventBus bus;
	 *
	 * // Subscribe to player join events
	 * bus.Subscribe(EventType::PlayerJoin,
	 *     [](Event& event) {
	 *         auto& joinEvent = static_cast<PlayerJoinEvent&>(event);
	 *         LOG_INFO("Player joined: {}", joinEvent.PlayerName());
	 *     },
	 *     EventPriority::Normal,
	 *     "MyPlugin"
	 * );
	 *
	 * // Publish an event
	 * PlayerJoinEvent event(player);
	 * bus.Publish(event);
	 * @endcode
	 */
	class EventBus {
	public:
		using EventCallback = std::function<void(Event&)>;

		/**
		 * Internal subscription structure. Maintains sort order by priority.
		 */
		struct Subscription {
			EventType type;           ///< The event type to listen for
			EventPriority priority;   ///< Handler invocation priority
			EventCallback callback;   ///< The handler function
			std::string owner;        ///< Plugin/module name for bulk unsubscribe
		};

		/**
		 * Subscribes a callback to a specific event type.
		 *
		 * @param type The event type to subscribe to.
		 * @param callback Function to invoke when events of this type are published.
		 * @param priority Handler priority (default: Normal).
		 * @param owner Plugin/module identifier for bulk unsubscribe (default: empty).
		 *
		 * @note The callback must not block for extended periods as it blocks
		 *       the event dispatch loop.
		 */
		void Subscribe(EventType type, EventCallback callback,
			EventPriority priority = EventPriority::Normal,
			const std::string& owner = "");

		/**
		 * Publishes an event to all matching subscribers.
		 *
		 * Events are dispatched synchronously in priority order. If an event
		 * is cancelled (IsHandled() returns true), only Monitor priority handlers
		 * will receive subsequent callbacks.
		 *
		 * @param event The event to publish. Must have a valid Type().
		 *
		 * @note This method is thread-safe but callbacks execute on the calling
		 *       thread. Ensure proper synchronization if handlers access shared state.
		 */
		void Publish(Event& event);

		/**
		 * Removes all subscriptions owned by a specific plugin/module.
		 *
		 * This is typically called during plugin disable to clean up event handlers.
		 *
		 * @param owner The owner identifier used during Subscribe().
		 */
		void UnsubscribeAll(const std::string& owner);

		/**
		 * Gets the total number of active subscriptions.
		 *
		 * @return Count of all registered event handlers.
		 */
		size_t SubscriptionCount() const {
			std::shared_lock lock(m_Mutex);
			return m_Subscriptions.size();
		}

	private:
		mutable std::shared_mutex m_Mutex;        ///< Protects subscriptions vector
		std::vector<Subscription> m_Subscriptions; ///< Sorted by priority (highest first)
	};

}
