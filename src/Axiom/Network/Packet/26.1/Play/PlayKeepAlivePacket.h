#pragma once

/**
 * @file PlayKeepAlivePacket.h
 *
 * Client responds to server's keep-alive ping with the same ID.
 */

#include "Axiom/Network/Packet/PacketMacros.h"

namespace Axiom {

PACKET_DECL_BEGIN(PlayKeepAlivePacket, Play, Serverbound::Play::KeepAlive)
    PACKET_FIELD_INT64(KeepAliveId)
PACKET_DECL_END()

}
