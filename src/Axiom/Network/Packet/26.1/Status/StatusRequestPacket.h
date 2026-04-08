#pragma once

#include "Axiom/Network/Packet/PacketVersioned.h"

#include <cstdint>

namespace Axiom {

PACKET_VERSIONED_SB(StatusRequestPacket, 775, Status, 0x00)
PACKET_VERSIONED_END(StatusRequestPacket, 775)

}
