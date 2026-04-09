#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Environment/Item/ItemStack.h"

#include <cstdint>
#include <vector>

namespace Axiom {

	/**
	 * Interface for anything that holds items
	 * in indexed slots (chests, hoppers,
	 * furnaces, player inventories, etc.).
	 */
	class Container {
	public:
		virtual ~Container() = default;

		/**
		 * Total number of slots
		 * in this container.
		 */
		virtual int32_t GetContainerSize() const = 0;

		/**
		 * Get the item stack in the
		 * given slot index.
		 */
		virtual const ItemStack& GetItem(int32_t slot) const = 0;

		/**
		 * Set the item stack in the
		 * given slot index.
		 */
		virtual void SetItem(int32_t slot, ItemStack stack) = 0;

		/**
		 * Remove and return the item
		 * stack from the given slot.
		 */
		virtual ItemStack RemoveItem(int32_t slot);

		/**
		 * Remove up to a given count from
		 * a slot, returning the removed
		 * portion as a new stack.
		 */
		virtual ItemStack RemoveItem(int32_t slot, int32_t count);

		/**
		 * Mark the container as modified,
		 * triggering persistence or
		 * network sync.
		 */
		virtual void SetChanged() = 0;

		/**
		 * Clear all slots in the
		 * container to empty.
		 */
		virtual void ClearContent() = 0;

		/**
		 * Check whether a specific slot
		 * index is within bounds.
		 */
		bool IsValidSlot(int32_t slot) const {
			return slot >= 0 && slot < GetContainerSize();
		}

		/**
		 * Check whether the container
		 * has no items at all.
		 */
		bool IsEmpty() const;
	};

	/**
	 * Simple fixed-size container backed
	 * by a vector of ItemStacks.
	 */
	class SimpleContainer : public Container {
	public:
		explicit SimpleContainer(int32_t size);
		~SimpleContainer() override = default;

		int32_t GetContainerSize() const override { return static_cast<int32_t>(m_Items.size()); }
		const ItemStack& GetItem(int32_t slot) const override;
		void SetItem(int32_t slot, ItemStack stack) override;
		void SetChanged() override;
		void ClearContent() override;

	private:
		std::vector<ItemStack> m_Items;
		bool m_Changed = false;
	};

}
