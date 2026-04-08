#pragma once

/**
 * @file PlayerActionPacket.h
 * @brief Player Action: digging (block breaking), drop item, swap hands, etc.
 *
 * Action values:
 * - 0 = Started digging
 * - 1 = Cancelled digging
 * - 2 = Finished digging
 */

#include "Axiom/Network/Packet/PacketVersioned.h"

namespace Axiom {

PACKET_VERSIONED_SB(PlayerActionPacket, 775, Play, Serverbound::Play::PlayerAction)
	FIELD(int32_t, Action, 0)
	FIELD(int32_t, BlockX, 0)
	FIELD(int32_t, BlockY, 0)
	FIELD(int32_t, BlockZ, 0)
	FIELD(uint8_t, Face, 0)
	FIELD(int32_t, Sequence, 0)
PACKET_VERSIONED_END(PlayerActionPacket, 775)

}
