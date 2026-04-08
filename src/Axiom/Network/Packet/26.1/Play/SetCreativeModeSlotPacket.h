#pragma once

/**
 * @file SetCreativeModeSlotPacket.h
 * @brief Creative mode inventory slot update.
 *
 * In creative mode, the client tells the server to put an item in a slot.
 * Used to track what the player is holding.
 */

#include "Axiom/Network/Packet/PacketVersioned.h"

namespace Axiom {

PACKET_VERSIONED_SB(SetCreativeModeSlotPacket, 775, Play, Serverbound::Play::SetCreativeModeSlot)
	FIELD(int16_t, SlotIndex, 0)
	FIELD(int32_t, ItemCount, 0)
	FIELD(int32_t, ItemId, 0)
PACKET_VERSIONED_END(SetCreativeModeSlotPacket, 775)

}
