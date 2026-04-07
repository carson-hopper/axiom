#pragma once

/**
 * @file ClientTickEndPacket.h
 *
 * Client signals end of tick processing.
 */

#include "Axiom/Network/Packet/PacketMacros.h"

namespace Axiom {

DEFINE_SIMPLE_PACKET(ClientTickEndPacket, Play, Serverbound::Play::ClientTickEnd)

}
