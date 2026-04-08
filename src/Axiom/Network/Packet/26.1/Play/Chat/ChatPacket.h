#pragma once

#include "Axiom/Network/Packet/PacketVersioned.h"

namespace Axiom {

PACKET_VERSIONED(ChatPacket, 775, Play, 0x09)
	FIELD_STRING(Message)
	FIELD(int64_t, Timestamp, 0)
	FIELD(int64_t, Salt, 0)
	FIELD_BYTE_ARRAY(256, Signature)
PACKET_VERSIONED_END(ChatPacket, 775)

}
