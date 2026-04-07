#pragma once

/**
 * @file UseItemOnPacket.h
 * @brief Player right-clicks on a block face to place a block or interact.
 */

#include "Axiom/Network/Packet/PacketMacros.h"

namespace Axiom {

PACKET_DECL_BEGIN(UseItemOnPacket, Play, Serverbound::Play::UseItemOn)
	PACKET_FIELD_INT32(Hand)
	PACKET_FIELD_INT32(BlockX)
	PACKET_FIELD_INT32(BlockY)
	PACKET_FIELD_INT32(BlockZ)
	PACKET_FIELD_INT32(Face)
	PACKET_FIELD_FLOAT(CursorX)
	PACKET_FIELD_FLOAT(CursorY)
	PACKET_FIELD_FLOAT(CursorZ)
	PACKET_FIELD_BOOL(InsideBlock)
	PACKET_FIELD_BOOL(WorldBorderHit)
	PACKET_FIELD_INT32(Sequence)
PACKET_DECL_END()

}