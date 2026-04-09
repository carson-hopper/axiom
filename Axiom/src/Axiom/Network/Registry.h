#pragma once

#include "Axiom/Network/ResourceLocation.h"

#include <cstdint>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Axiom {

	/**
	 * Generic registry that maps ResourceLocation
	 * keys to entries with sequential
	 * integer IDs assigned
	 * at registration.
	 */
	template<typename T>
	class Registry {
	public:
		explicit Registry(ResourceLocation name)
			: m_Name(std::move(name)) {}

		/**
		 * Register an entry and return
		 * its numeric ID.
		 */
		int32_t Register(ResourceLocation key, T value) {
			if (m_Frozen) {
				throw std::runtime_error(
					"Cannot register entry in frozen registry: " + m_Name.ToString());
			}

			auto existing = m_IdByKey.find(key);
			if (existing != m_IdByKey.end()) {
				throw std::runtime_error(
					"Duplicate registry entry: " + key.ToString() +
					" in " + m_Name.ToString());
			}

			int32_t identifier = static_cast<int32_t>(m_Entries.size());
			m_Entries.emplace_back(key, std::move(value));
			m_IdByKey.emplace(std::move(key), identifier);
			return identifier;
		}

		/**
		 * Get a mutable pointer to
		 * an entry by key.
		 */
		T* Get(const ResourceLocation& key) {
			auto iterator = m_IdByKey.find(key);
			if (iterator == m_IdByKey.end()) {
				return nullptr;
			}
			return &m_Entries[iterator->second].second;
		}

		/**
		 * Get a const pointer to
		 * an entry by key.
		 */
		const T* Get(const ResourceLocation& key) const {
			auto iterator = m_IdByKey.find(key);
			if (iterator == m_IdByKey.end()) {
				return nullptr;
			}
			return &m_Entries[iterator->second].second;
		}

		/**
		 * Get a mutable pointer to
		 * an entry by numeric ID.
		 */
		T* GetById(int32_t identifier) {
			if (identifier < 0 || identifier >= static_cast<int32_t>(m_Entries.size())) {
				return nullptr;
			}
			return &m_Entries[identifier].second;
		}

		/**
		 * Get a const pointer to
		 * an entry by numeric ID.
		 */
		const T* GetById(int32_t identifier) const {
			if (identifier < 0 || identifier >= static_cast<int32_t>(m_Entries.size())) {
				return nullptr;
			}
			return &m_Entries[identifier].second;
		}

		/**
		 * Get the numeric ID for a key.
		 * Returns -1 if not found.
		 */
		int32_t GetId(const ResourceLocation& key) const {
			auto iterator = m_IdByKey.find(key);
			if (iterator == m_IdByKey.end()) {
				return -1;
			}
			return iterator->second;
		}

		/**
		 * Freeze the registry so no
		 * more entries can
		 * be registered.
		 */
		void Freeze() { m_Frozen = true; }

		/**
		 * Returns true if the registry
		 * has been frozen.
		 */
		bool IsFrozen() const { return m_Frozen; }

		const ResourceLocation& Name() const { return m_Name; }
		int32_t Size() const { return static_cast<int32_t>(m_Entries.size()); }

		auto begin() const { return m_Entries.begin(); }
		auto end() const { return m_Entries.end(); }

	private:
		ResourceLocation m_Name;
		std::vector<std::pair<ResourceLocation, T>> m_Entries;
		std::unordered_map<ResourceLocation, int32_t> m_IdByKey;
		bool m_Frozen = false;
	};

}
