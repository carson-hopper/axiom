#pragma once

#include "Axiom/Network/Packet/PacketVersioned.h"

#include <string>

namespace Axiom {

PACKET_VERSIONED(LoginHelloPacket, 775, Serverbound, Login, 0x00)
	FIELD_STRING(PlayerName)
PACKET_VERSIONED_END(LoginHelloPacket, 775)

}
