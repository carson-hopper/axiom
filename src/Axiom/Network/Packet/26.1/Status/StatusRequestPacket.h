#pragma once

/**
 * @file StatusRequestPacket.h
 * @brief Status request packet (server list ping).
 *
 * Sent by the client to request server status information (MOTD, player count, etc.)
 */

#include "Axiom/Network/Packet/PacketMacros.h"

namespace Axiom {

	DEFINE_SIMPLE_PACKET(StatusRequestPacket, Status, Serverbound::Status::StatusRequest)

}