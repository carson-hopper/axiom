#pragma once

/**
 * @file AcceptTeleportationPacket.h
 *
 * Client confirms it has teleported to the requested position.
 */

#include "Axiom/Network/Packet/PacketVersioned.h"

namespace Axiom {

PACKET_VERSIONED_SB(AcceptTeleportationPacket, 775, Play, Serverbound::Play::AcceptTeleportation)
	FIELD(int32_t, TeleportId, 0)
PACKET_VERSIONED_END(AcceptTeleportationPacket, 775)

}
