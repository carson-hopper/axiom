#pragma once

#include "Axiom/Network/Packet/PacketVersioned.h"

namespace Axiom {

PACKET_VERSIONED(LoginAcknowledgedPacket, 775, Serverbound, Login, 0x03)
PACKET_VERSIONED_END(LoginAcknowledgedPacket, 775)

}
