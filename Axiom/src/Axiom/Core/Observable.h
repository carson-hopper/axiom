#pragma once

#include "Axiom/Core/Base.h"

#include <concepts>
#include <cstdint>
#include <functional>
#include <mutex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Axiom {

	/**
	 * A reactive wrapper around a value. Every time the value is
	 * assigned to something different, every registered listener
	 * is invoked with the old and new values.
	 *
	 * Typical usage is to expose observable state on an entity or
	 * manager class and let other systems subscribe to it. Listeners
	 * that fire EventBus events can be installed by the owning class
	 * to bridge local observation with the global event bus.
	 *
	 * Example:
	 * @code
	 * class Player {
	 * public:
	 *     Observable<int> Health{20};
	 *
	 *     Player(EventBus& bus) {
	 *         Health.Subscribe([&bus, this](int oldValue, int newValue) {
	 *             PlayerHealthChangedEvent event(this, oldValue, newValue);
	 *             bus.Publish(event);
	 *         });
	 *     }
	 * };
	 *
	 * Usage:
	 *     player.Health = 15;         // triggers every listener
	 *     int hp = player.Health;     // implicit conversion
	 *     int hp2 = player.Health.Get();
	 * @endcode
	 */
	template<typename T>
	class Observable {
	public:
		using Listener = std::function<void(const T& oldValue, const T& newValue)>;
		using ListenerId = uint32_t;

		Observable() requires std::default_initializable<T> = default;
		explicit Observable(T initial) : m_Value(std::move(initial)) {}

		Observable(const Observable&) = delete;
		Observable& operator=(const Observable&) = delete;
		Observable(Observable&&) = delete;
		Observable& operator=(Observable&&) = delete;

		/**
		 * Read the current value. Returns a copy — reading a
		 * reference would be racy against a concurrent Set.
		 */
		T Get() const {
			std::lock_guard<std::mutex> const lock(m_Mutex);
			return m_Value;
		}

		/**
		 * Assign a new value. If the value compares equal to the
		 * current one (where applicable), no listeners are invoked.
		 * Otherwise, every registered listener is called with the
		 * old and new values.
		 *
		 * Listeners are invoked *without* the internal mutex held,
		 * so a listener may safely call Subscribe / Unsubscribe /
		 * Set on the same Observable without deadlocking. A listener
		 * added or removed during dispatch takes effect on the
		 * next Set, not the current one.
		 */
		void Set(T value) {
			T oldValue;
			T newValue;
			std::vector<Listener> snapshot;
			{
				std::lock_guard<std::mutex> const lock(m_Mutex);
				if constexpr (std::equality_comparable<T>) {
					if (m_Value == value) {
						return;
					}
				}
				oldValue = std::move(m_Value);
				m_Value = std::move(value);
				newValue = m_Value;
				snapshot.reserve(m_Listeners.size());
				for (const auto& [listenerId, listener] : m_Listeners) {
					snapshot.push_back(listener);
				}
			}

			for (const auto& listener : snapshot) {
				listener(oldValue, newValue);
			}
		}

		/**
		 * Register a listener. Returns an opaque ID that can be
		 * passed to Unsubscribe to remove the listener later.
		 */
		ListenerId Subscribe(Listener listener) {
			std::lock_guard<std::mutex> const lock(m_Mutex);
			const ListenerId id = m_NextId++;
			m_Listeners[id] = std::move(listener);
			return id;
		}

		/** Remove a previously registered listener. */
		void Unsubscribe(const ListenerId id) {
			std::lock_guard<std::mutex> const lock(m_Mutex);
			m_Listeners.erase(id);
		}

		/** Number of registered listeners. */
		size_t ListenerCount() const {
			std::lock_guard<std::mutex> const lock(m_Mutex);
			return m_Listeners.size();
		}

		/**
		 * Implicit conversion to T. Returns by value so the read
		 * is race-free against a concurrent Set — any caller
		 * expecting a stable T&amp; must keep their own copy.
		 */
		operator T() const { return Get(); }

		Observable& operator=(T value) {
			Set(std::move(value));
			return *this;
		}

	private:
		T m_Value{};
		ListenerId m_NextId = 1;
		std::unordered_map<ListenerId, Listener> m_Listeners;
		mutable std::mutex m_Mutex;
	};

}
