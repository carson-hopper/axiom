#pragma once

/**
 * @file SetCarriedItemPacket.h
 *
 * Client tells the server which hotbar slot is selected (0-8).
 */

#include "Axiom/Network/Packet/PacketVersioned.h"

namespace Axiom {

PACKET_VERSIONED_SB(SetCarriedItemPacket, 775, Play, Serverbound::Play::SetCarriedItem)
	FIELD(int32_t, Slot, 0)
PACKET_VERSIONED_END(SetCarriedItemPacket, 775)

}
