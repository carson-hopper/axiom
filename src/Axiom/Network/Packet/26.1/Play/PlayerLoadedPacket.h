#pragma once

/**
 * @file PlayerLoadedPacket.h
 *
 * Client signals that it has finished loading the world.
 */

#include "Axiom/Network/Packet/PacketVersioned.h"

namespace Axiom {

PACKET_VERSIONED_SB(PlayerLoadedPacket, 775, Play, Serverbound::Play::PlayerLoaded)
PACKET_VERSIONED_END(PlayerLoadedPacket, 775)

}
