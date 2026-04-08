#pragma once

/**
 * @file FinishConfigurationPacket.h
 *
 * Client acknowledges configuration is complete and is ready to enter play.
 */

#include "Axiom/Network/Packet/PacketVersioned.h"

#include <cstdint>

namespace Axiom {

PACKET_VERSIONED(FinishConfigurationPacket, 775, Serverbound, Configuration, 0)
PACKET_VERSIONED_END(FinishConfigurationPacket, 775)

}
