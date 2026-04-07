#include "SetCreativeModeSlotPacket.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom {

	// ----- Packet Decoding ------------------------------------------

	PACKET_DECODE_BEGIN(SetCreativeModeSlotPacket)
		READ_SHORT(m_SlotIndex);

		// Slot data: read the item count and ID
		// In 26.1, slot format is: VarInt count, then if count > 0:
		//   VarInt itemId, VarInt componentsToAdd, VarInt componentsToRemove, ...
		READ_VARINT(m_ItemCount);
		if (m_ItemCount > 0) {
			READ_VARINT(m_ItemId);
			// Skip remaining component data
			const int32_t componentsToAdd = buffer.ReadVarInt();
			const int32_t componentsToRemove = buffer.ReadVarInt();
			// We don't need the component data for block placement
			// Just consume the rest of the buffer
			for (int32_t i = 0; i < componentsToAdd; i++) {
				buffer.ReadVarInt(); // Component type
				// Skip component data — varies by type, consume remaining
				break; // Simplified: just read the first and stop
			}
		}
	PACKET_DECODE_END()

	// ----- Packet Handling ------------------------------------------

	PACKET_HANDLE_BEGIN(SetCreativeModeSlotPacket)
		auto player = context.Players().GetPlayer(connection->Id());
		if (!player) return;

		// Hotbar slots are inventory slots 36-44
		// Slot index in creative packet is the absolute inventory slot
		if (m_SlotIndex >= 36 && m_SlotIndex <= 44) {
			const int hotbarIndex = m_SlotIndex - 36;
			player->SetHotbarItem(hotbarIndex, m_ItemId, m_ItemCount);
			AX_CORE_TRACE("Creative slot {}: item {} x{}", hotbarIndex, m_ItemId, m_ItemCount);
		}
	PACKET_HANDLE_END()

	// ----- Template Instantiation -----------------------------------

	PACKET_INSTANTIATE(SetCreativeModeSlotPacket, 775)

}