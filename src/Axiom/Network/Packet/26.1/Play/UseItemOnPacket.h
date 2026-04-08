#pragma once

/**
 * @file UseItemOnPacket.h
 * @brief Player right-clicks on a block face to place a block or interact.
 */

#include "Axiom/Network/Packet/PacketVersioned.h"

namespace Axiom {

PACKET_VERSIONED_SB(UseItemOnPacket, 775, Play, Serverbound::Play::UseItemOn)
	FIELD(int32_t, Hand, 0)
	FIELD(int32_t, BlockX, 0)
	FIELD(int32_t, BlockY, 0)
	FIELD(int32_t, BlockZ, 0)
	FIELD(int32_t, Face, 0)
	FIELD(float, CursorX, 0)
	FIELD(float, CursorY, 0)
	FIELD(float, CursorZ, 0)
	FIELD(bool, InsideBlock, 0)
	FIELD(bool, WorldBorderHit, 0)
	FIELD(int32_t, Sequence, 0)
PACKET_VERSIONED_END(UseItemOnPacket, 775)

}
