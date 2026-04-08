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

#include "Axiom/Network/Packet/PacketMacros.h"

namespace Axiom {

	PACKET_DECL_BEGIN(PlayerActionPacket, Play, Serverbound::Play::PlayerAction)
		PACKET_FIELD_INT32(Action)
		PACKET_FIELD_COORDS(Block)
		PACKET_FIELD_UINT8(Face)
		PACKET_FIELD_INT32(Sequence)
	PACKET_DECL_END()

}
