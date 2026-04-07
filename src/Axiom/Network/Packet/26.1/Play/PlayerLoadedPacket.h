#pragma once

/**
 * @file PlayerLoadedPacket.h
 *
 * Client signals that it has finished loading the world.
 */

#include "Axiom/Network/Packet/PacketMacros.h"

namespace Axiom {

DEFINE_SIMPLE_PACKET(PlayerLoadedPacket, Play, Serverbound::Play::PlayerLoaded)

}
