#include "ItemStack.h"

#include <algorithm>

namespace Axiom {

	const ItemStack ItemStack::EMPTY{};

	ItemStack::ItemStack(int32_t itemId, int32_t count)
		: m_ItemId(itemId)
		, m_Count(count) {}

	void ItemStack::SetCount(int32_t count) {
		m_Count = count;
	}

	ItemStack ItemStack::Split(int32_t amount) {
		int32_t taken = std::min(amount, m_Count);
		ItemStack result(m_ItemId, taken);
		result.m_Components = m_Components;
		m_Count -= taken;
		return result;
	}

	ItemStack ItemStack::Merge(ItemStack& other, int32_t maxStackSize) {
		if (other.IsEmpty() || (m_ItemId != 0 && m_ItemId != other.m_ItemId)) {
			return other;
		}

		if (m_ItemId == 0) {
			m_ItemId = other.m_ItemId;
			m_Components = other.m_Components;
		}

		int32_t spaceAvailable = maxStackSize - m_Count;
		int32_t transfer = std::min(spaceAvailable, other.m_Count);
		m_Count += transfer;
		other.m_Count -= transfer;

		return other;
	}

	void ItemStack::SetComponent(const std::string& key, const std::string& value) {
		m_Components[key] = value;
	}

	std::string ItemStack::GetComponent(const std::string& key, const std::string& fallback) const {
		auto iterator = m_Components.find(key);
		if (iterator != m_Components.end()) {
			return iterator->second;
		}
		return fallback;
	}

	bool ItemStack::HasComponent(const std::string& key) const {
		return m_Components.contains(key);
	}

	void ItemStack::RemoveComponent(const std::string& key) {
		m_Components.erase(key);
	}

	bool ItemStack::operator==(const ItemStack& other) const {
		return m_ItemId == other.m_ItemId
			&& m_Count == other.m_Count
			&& m_Components == other.m_Components;
	}

}
