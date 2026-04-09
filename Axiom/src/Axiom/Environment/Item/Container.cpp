#include "Container.h"

#include "Axiom/Core/Log.h"

namespace Axiom {

	ItemStack Container::RemoveItem(int32_t slot) {
		if (!IsValidSlot(slot)) {
			return ItemStack::EMPTY;
		}

		ItemStack removed = GetItem(slot);
		SetItem(slot, ItemStack{});
		SetChanged();
		return removed;
	}

	ItemStack Container::RemoveItem(int32_t slot, int32_t count) {
		if (!IsValidSlot(slot)) {
			return ItemStack::EMPTY;
		}

		const ItemStack& current = GetItem(slot);
		if (current.IsEmpty()) {
			return ItemStack::EMPTY;
		}

		int32_t toRemove = std::min(count, current.Count());
		ItemStack removed(current.ItemId(), toRemove);

		if (toRemove >= current.Count()) {
			SetItem(slot, ItemStack{});
		} else {
			ItemStack remaining(current.ItemId(), current.Count() - toRemove);
			SetItem(slot, remaining);
		}

		SetChanged();
		return removed;
	}

	bool Container::IsEmpty() const {
		for (int32_t slot = 0; slot < GetContainerSize(); ++slot) {
			if (!GetItem(slot).IsEmpty()) {
				return false;
			}
		}
		return true;
	}

	// ----- SimpleContainer ------------------------------------------

	SimpleContainer::SimpleContainer(int32_t size)
		: m_Items(static_cast<size_t>(size)) {
	}

	const ItemStack& SimpleContainer::GetItem(int32_t slot) const {
		if (!IsValidSlot(slot)) {
			return ItemStack::EMPTY;
		}
		return m_Items[static_cast<size_t>(slot)];
	}

	void SimpleContainer::SetItem(int32_t slot, ItemStack stack) {
		if (!IsValidSlot(slot)) {
			AX_CORE_WARN("SimpleContainer: slot {} out of range (size {})",
				slot, GetContainerSize());
			return;
		}
		m_Items[static_cast<size_t>(slot)] = std::move(stack);
		SetChanged();
	}

	void SimpleContainer::SetChanged() {
		m_Changed = true;
	}

	void SimpleContainer::ClearContent() {
		for (auto& item : m_Items) {
			item = ItemStack{};
		}
		SetChanged();
	}

}
