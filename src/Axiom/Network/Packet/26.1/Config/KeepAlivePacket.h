#pragma once

/**
 * @file KeepAlivePacket.h
 *
 * Client responds to server's keep-alive ping during configuration.
 */

#include "Axiom/Network/Packet/PacketMacros.h"

namespace Axiom {

PACKET_DECL_BEGIN(ConfigKeepAlivePacket, Configuration, Serverbound::Config::KeepAlive)
    PACKET_FIELD_INT64(KeepAliveId)
PACKET_DECL_END()

}
