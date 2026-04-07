#pragma once

/**
 * @file AcceptTeleportationPacket.h
 *
 * Client confirms it has teleported to the requested position.
 */

#include "Axiom/Network/Packet/PacketMacros.h"

namespace Axiom {

PACKET_DECL_BEGIN(AcceptTeleportationPacket, Play, Serverbound::Play::AcceptTeleportation)
    PACKET_FIELD_INT32(TeleportId)
PACKET_DECL_END()

}
