#include "ContainerMenu.h"

#include "Axiom/Core/Log.h"

namespace Axiom {

	ContainerMenu::ContainerMenu(int32_t containerId)
		: m_ContainerId(containerId) {
	}

	void ContainerMenu::AddSlot(Container* container, int32_t containerSlot) {
		MenuSlot slot;
		slot.container = container;
		slot.containerSlot = containerSlot;
		slot.menuSlotIndex = static_cast<int32_t>(m_Slots.size());
		m_Slots.push_back(slot);
	}

	const MenuSlot* ContainerMenu::GetSlot(int32_t menuSlotIndex) const {
		if (menuSlotIndex < 0 || menuSlotIndex >= static_cast<int32_t>(m_Slots.size())) {
			return nullptr;
		}
		return &m_Slots[static_cast<size_t>(menuSlotIndex)];
	}

	void ContainerMenu::OnSlotClick(int32_t slotIndex, int32_t clickType, ItemStack& carriedItem) {
		AX_CORE_TRACE("ContainerMenu: slot click — slot={}, type={}", slotIndex, clickType);

		// Stub: real implementation would handle left-click
		// pickup/place, right-click split, and other click
		// modes based on the clickType
		(void)carriedItem;
	}

	ItemStack ContainerMenu::QuickMoveStack(int32_t slotIndex) {
		AX_CORE_TRACE("ContainerMenu: quick-move slot {}", slotIndex);

		// Stub: real implementation would shift-click move
		// items between container and player inventory
		(void)slotIndex;
		return ItemStack{};
	}

	int32_t ContainerMenu::AddDataSlot(int32_t initialValue) {
		DataSlot dataSlot;
		dataSlot.value = initialValue;
		dataSlot.previousValue = initialValue;
		m_DataSlots.push_back(dataSlot);
		return static_cast<int32_t>(m_DataSlots.size()) - 1;
	}

	int32_t ContainerMenu::GetDataSlotValue(int32_t dataSlotIndex) const {
		if (dataSlotIndex < 0 || dataSlotIndex >= static_cast<int32_t>(m_DataSlots.size())) {
			return 0;
		}
		return m_DataSlots[static_cast<size_t>(dataSlotIndex)].value;
	}

	void ContainerMenu::SetDataSlotValue(int32_t dataSlotIndex, int32_t value) {
		if (dataSlotIndex < 0 || dataSlotIndex >= static_cast<int32_t>(m_DataSlots.size())) {
			AX_CORE_WARN("ContainerMenu: data slot {} out of range", dataSlotIndex);
			return;
		}
		m_DataSlots[static_cast<size_t>(dataSlotIndex)].value = value;
	}

	std::vector<std::pair<int32_t, int32_t>> ContainerMenu::CollectChangedDataSlots() {
		std::vector<std::pair<int32_t, int32_t>> changed;

		for (size_t index = 0; index < m_DataSlots.size(); ++index) {
			auto& dataSlot = m_DataSlots[index];
			if (dataSlot.value != dataSlot.previousValue) {
				changed.emplace_back(static_cast<int32_t>(index), dataSlot.value);
				dataSlot.previousValue = dataSlot.value;
			}
		}

		return changed;
	}

}
