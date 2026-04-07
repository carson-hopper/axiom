#include "SetCreativeModeSlotPacket.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom {

	template<int32_t Version>
	void SetCreativeModeSlotPacket<Version>::Handle(const Ref<Connection> connection, PacketContext& context) {
		auto player = context.Players().GetPlayer(connection.get());
		if (!player) return;

		// Hotbar slots are inventory slots 36-44
		// Slot index in creative packet is the absolute inventory slot
		if (m_SlotIndex >= 36 && m_SlotIndex <= 44) {
			const int hotbarIndex = m_SlotIndex - 36;
			player->SetHotbarItem(hotbarIndex, m_ItemId, m_ItemCount);
			AX_CORE_TRACE("Creative slot {}: item {} x{}", hotbarIndex, m_ItemId, m_ItemCount);
		}
	}

	template class SetCreativeModeSlotPacket<775>;

}
