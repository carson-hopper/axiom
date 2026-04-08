#pragma once

#include "Axiom/Network/Packet/PacketVersioned.h"

#include <cstdint>
#include <string>

namespace Axiom {

PACKET_VERSIONED(HandshakePacket, 775, Serverbound, Handshake, 0x00)
	FIELD(int32_t, ProtocolVersion)
	FIELD(std::string, ServerAddress)
	FIELD(uint16_t, ServerPort)
	FIELD(int32_t, NextState)
PACKET_VERSIONED_END(HandshakePacket, 775)

}
