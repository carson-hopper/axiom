#pragma once

#include "Axiom/Core/Base.h"

#include <cstdint>
#include <string>

namespace Axiom {

	/**
	 * Represents an item type in the game.
	 * An Item defines the properties of a type of item (diamond sword,
	 * stick, etc.) It is NOT a stack instance — see ItemStack for that.
	 */
	class Item {
	public:
		Item(const int32_t itemId, std::string name)
			: m_ItemId(itemId)
			, m_Name(std::move(name)) {}

		virtual ~Item() = default;

		int32_t GetItemId() const { return m_ItemId; }
		const std::string& Name() const { return m_Name; }

		int GetMaxStackSize() const { return m_MaxStackSize; }
		void SetMaxStackSize(int size) { m_MaxStackSize = size; }

		int GetMaxDamage() const { return m_MaxDamage; }
		void SetMaxDamage(const int damage) { m_MaxDamage = damage; }

		bool IsStackable() const { return m_MaxStackSize > 1; }

	private:
		int32_t m_ItemId;
		std::string m_Name;
		int m_MaxStackSize = 64;
		int m_MaxDamage = 0;
	};

}
