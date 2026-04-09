#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

namespace Axiom {

	/**
	 * Represents a stack of items in an inventory
	 * slot, carrying a count and arbitrary string-
	 * keyed data components (1.21 component model).
	 */
	class ItemStack {
	public:
		ItemStack() = default;
		ItemStack(int32_t itemId, int32_t count = 1);

		int32_t ItemId() const { return m_ItemId; }
		int32_t Count() const { return m_Count; }
		void SetCount(int32_t count);

		bool IsEmpty() const { return m_Count <= 0 || m_ItemId == 0; }

		/**
		 * Split off a number of
		 * items from this stack.
		 */
		ItemStack Split(int32_t amount);

		/**
		 * Merge another stack into this one.
		 * Returns the leftover items.
		 */
		ItemStack Merge(ItemStack& other, int32_t maxStackSize = 64);

		void SetComponent(const std::string& key, const std::string& value);
		std::string GetComponent(const std::string& key, const std::string& fallback = "") const;
		bool HasComponent(const std::string& key) const;
		void RemoveComponent(const std::string& key);

		bool operator==(const ItemStack& other) const;

		static const ItemStack EMPTY;

	private:
		int32_t m_ItemId = 0;
		int32_t m_Count = 0;
		std::unordered_map<std::string, std::string> m_Components;
	};

}
