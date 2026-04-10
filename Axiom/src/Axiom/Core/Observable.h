#pragma once

#include "Axiom/Core/Base.h"

#include <concepts>
#include <cstdint>
#include <functional>
#include <unordered_map>
#include <utility>

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

		Observable() = default;
		explicit Observable(T initial) : m_Value(std::move(initial)) {}

		Observable(const Observable&) = delete;
		Observable& operator=(const Observable&) = delete;
		Observable(Observable&&) = delete;
		Observable& operator=(Observable&&) = delete;

		~Observable() {
			for (const auto& [id, listener] : m_Listeners) {
				Unsubscribe(id);
			}
		}

		/** Read the current value. */
		const T& Get() const { return m_Value; }

		/**
		 * Assign a new value. If the value compares equal to the
		 * current one (where applicable), no listeners are invoked.
		 * Otherwise, every registered listener is called with the
		 * old and new values.
		 */
		void Set(T value) {
			if constexpr (std::equality_comparable<T>) {
				if (m_Value == value) return;
			}
			T oldValue = std::move(m_Value);
			m_Value = std::move(value);
			for (auto& [id, listener] : m_Listeners) {
				listener(oldValue, m_Value);
			}
		}

		/**
		 * Register a listener. Returns an opaque ID that can be
		 * passed to Unsubscribe to remove the listener later.
		 */
		ListenerId Subscribe(Listener listener) {
			const ListenerId id = m_NextId++;
			m_Listeners[id] = std::move(listener);
			return id;
		}

		/** Remove a previously registered listener. */
		void Unsubscribe(const ListenerId id) {
			m_Listeners.erase(id);
		}

		/** Number of registered listeners. */
		size_t ListenerCount() const { return m_Listeners.size(); }

		// Convenience operators so existing code can still read the
		// observable the same way it reads a plain member variable.
		operator const T&() const { return m_Value; }
		const T* operator->() const { return &m_Value; }

		Observable& operator=(T value) {
			Set(std::move(value));
			return *this;
		}

	private:
		T m_Value{};
		ListenerId m_NextId = 1;
		std::unordered_map<ListenerId, Listener> m_Listeners;
	};

}
