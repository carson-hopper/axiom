#pragma once

#include "Axiom/Network/Packet/PacketVersioned.h"

#include <cstdint>

namespace Axiom {

PACKET_VERSIONED_SB(PingRequestPacket, 775, Status, 0x01)
	FIELD(int64_t, Payload, 0)
PACKET_VERSIONED_END(PingRequestPacket, 775)

}
