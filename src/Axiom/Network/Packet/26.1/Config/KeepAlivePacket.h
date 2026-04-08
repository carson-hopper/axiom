#pragma once

/**
 * @file KeepAlivePacket.h
 *
 * Client responds to server's keep-alive ping during configuration.
 */

#include "Axiom/Network/Packet/PacketVersioned.h"

#include <cstdint>

namespace Axiom {

PACKET_VERSIONED(ConfigKeepAlivePacket, 775, Serverbound, Configuration, 0)
	FIELD(int64_t, Id, 0)
PACKET_VERSIONED_END(ConfigKeepAlivePacket, 775)

}
