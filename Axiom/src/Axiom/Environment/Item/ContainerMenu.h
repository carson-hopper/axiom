#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Environment/Item/Container.h"
#include "Axiom/Environment/Item/ItemStack.h"

#include <cstdint>
#include <vector>

namespace Axiom {

	/**
	 * A slot within a container menu,
	 * mapping a visual slot index to
	 * a container and container slot.
	 */
	struct MenuSlot {
		Container* container = nullptr;
		int32_t containerSlot = 0;
		int32_t menuSlotIndex = 0;
	};

	/**
	 * A data slot for syncing integer
	 * values (furnace progress, brew
	 * time, enchant level, etc.).
	 */
	struct DataSlot {
		int32_t value = 0;
		int32_t previousValue = 0;
	};

	/**
	 * Base class for container UI menus.
	 * Manages the mapping between visual
	 * slot indices and underlying container
	 * slots, click handling, and data
	 * slot synchronization.
	 */
	class ContainerMenu {
	public:
		explicit ContainerMenu(int32_t containerId);
		virtual ~ContainerMenu() = default;

		int32_t ContainerId() const { return m_ContainerId; }

		// ----- Slot management ------------------------------------------

		/**
		 * Add a slot linking a container
		 * position to this menu.
		 */
		void AddSlot(Container* container, int32_t containerSlot);

		/**
		 * Get the menu slot at the
		 * given visual index.
		 */
		const MenuSlot* GetSlot(int32_t menuSlotIndex) const;

		/**
		 * Total number of slots
		 * in this menu.
		 */
		int32_t SlotCount() const { return static_cast<int32_t>(m_Slots.size()); }

		// ----- Click handling stub --------------------------------------

		/**
		 * Handle a click action on a slot.
		 * Click type: 0 = left, 1 = right,
		 * 2 = middle, etc.
		 */
		virtual void OnSlotClick(int32_t slotIndex, int32_t clickType, ItemStack& carriedItem);

		/**
		 * Handle a quick-move (shift-click)
		 * action on a slot.
		 */
		virtual ItemStack QuickMoveStack(int32_t slotIndex);

		// ----- Data slots -----------------------------------------------

		/**
		 * Add an integer data slot for
		 * syncing progress values etc.
		 */
		int32_t AddDataSlot(int32_t initialValue = 0);

		/**
		 * Get the current value of
		 * a data slot.
		 */
		int32_t GetDataSlotValue(int32_t dataSlotIndex) const;

		/**
		 * Set a data slot value,
		 * marking it dirty.
		 */
		void SetDataSlotValue(int32_t dataSlotIndex, int32_t value);

		/**
		 * Check which data slots have
		 * changed since last sync and
		 * clear their dirty flags.
		 */
		std::vector<std::pair<int32_t, int32_t>> CollectChangedDataSlots();

		// ----- Lifecycle ------------------------------------------------

		/**
		 * Called when the menu is removed
		 * (player closes the UI).
		 */
		virtual void OnRemoved() {}

	private:
		int32_t m_ContainerId;
		std::vector<MenuSlot> m_Slots;
		std::vector<DataSlot> m_DataSlots;
	};

}
