#include "axpch.h"
#include "Axiom/Core/TickRegistry.h"

#include <algorithm>

namespace Axiom {

	void TickRegistry::RegisterTickable(Ref<Tickable> tickable) {
		std::lock_guard<std::mutex> lock(m_Mutex);

		auto iterator = std::find(m_Tickables.begin(), m_Tickables.end(), tickable);
		if (iterator == m_Tickables.end()) {
			m_Tickables.push_back(std::move(tickable));
			m_TickablesDirty = true;
			m_Tickables.back()->OnTickRegistered();
		}
	}

	void TickRegistry::UnregisterTickable(Tickable* tickable) {
		std::lock_guard<std::mutex> lock(m_Mutex);

		auto iterator = std::find_if(m_Tickables.begin(), m_Tickables.end(),
			[tickable](const Ref<Tickable>& reference) {
				return reference.get() == tickable;
			});

		if (iterator != m_Tickables.end()) {
			(*iterator)->OnTickUnregistered();
			m_Tickables.erase(iterator);
			m_TickablesDirty = true;
		}
	}

	void TickRegistry::RegisterAsyncTickable(Ref<AsyncTickable> asyncTickable) {
		std::lock_guard<std::mutex> lock(m_Mutex);

		auto iterator = std::find(m_AsyncTickables.begin(), m_AsyncTickables.end(), asyncTickable);
		if (iterator == m_AsyncTickables.end()) {
			m_AsyncTickables.push_back(std::move(asyncTickable));
		}
	}

	void TickRegistry::UnregisterAsyncTickable(AsyncTickable* asyncTickable) {
		std::lock_guard<std::mutex> lock(m_Mutex);

		auto iterator = std::find_if(m_AsyncTickables.begin(), m_AsyncTickables.end(),
			[asyncTickable](const Ref<AsyncTickable>& reference) {
				return reference.get() == asyncTickable;
			});

		if (iterator != m_AsyncTickables.end()) {
			m_AsyncTickables.erase(iterator);
		}
	}

	std::vector<Ref<Tickable>> TickRegistry::GetTickables() {
		std::lock_guard<std::mutex> lock(m_Mutex);

		if (m_TickablesDirty) {
			std::sort(m_Tickables.begin(), m_Tickables.end(),
				[](const Ref<Tickable>& first, const Ref<Tickable>& second) {
					return first->GetTickPhase() < second->GetTickPhase();
				});
			m_TickablesDirty = false;
		}

		std::vector<Ref<Tickable>> enabledTickables;
		enabledTickables.reserve(m_Tickables.size());

		for (const auto& tickable : m_Tickables) {
			if (tickable && tickable->IsTickEnabled()) {
				enabledTickables.push_back(tickable);
			}
		}

		return enabledTickables;
	}

	std::vector<Ref<AsyncTickable>> TickRegistry::GetAsyncTickables() {
		std::lock_guard<std::mutex> lock(m_Mutex);

		std::vector<Ref<AsyncTickable>> enabledAsyncTickables;
		enabledAsyncTickables.reserve(m_AsyncTickables.size());

		for (const auto& asyncTickable : m_AsyncTickables) {
			if (asyncTickable && asyncTickable->IsAsyncEnabled()) {
				enabledAsyncTickables.push_back(asyncTickable);
			}
		}

		return enabledAsyncTickables;
	}

}
