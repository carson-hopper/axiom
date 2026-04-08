#pragma once

#include "Axiom/Network/Packet/PacketVersioned.h"

namespace Axiom {

PACKET_VERSIONED_SB(PlayKeepAlivePacket, 775, Play, 0x24)
	FIELD(int64_t, KeepAliveId, 0)
PACKET_VERSIONED_END(PlayKeepAlivePacket, 775)

}
