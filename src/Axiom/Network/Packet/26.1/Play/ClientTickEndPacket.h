#pragma once

/**
 * @file ClientTickEndPacket.h
 *
 * Client signals end of tick processing.
 */

#include "Axiom/Network/Packet/PacketVersioned.h"

namespace Axiom {

PACKET_VERSIONED_SB(ClientTickEndPacket, 775, Play, Serverbound::Play::ClientTickEnd)
PACKET_VERSIONED_END(ClientTickEndPacket, 775)

}
