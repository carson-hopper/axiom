#pragma once

/**
 * @file SetCreativeModeSlotPacket.h
 * @brief Creative mode inventory slot update.
 *
 * In creative mode, the client tells the server to put an item in a slot.
 * Used to track what the player is holding.
 */

#include "Axiom/Network/Packet/PacketMacros.h"

#include <cstdint>

namespace Axiom {

PACKET_DECL_BEGIN(SetCreativeModeSlotPacket, Play, Serverbound::Play::SetCreativeModeSlot)
	PACKET_FIELD_INT16(SlotIndex)
	PACKET_FIELD_INT32(ItemCount)
	PACKET_FIELD_INT32(ItemId)
PACKET_DECL_END()

}