#pragma once

/**
 * @file PingRequestPacket.h
 * @brief Ping request packet for latency measurement.
 */

#include "Axiom/Network/Packet/PacketMacros.h"

namespace Axiom {

	PACKET_DECL_BEGIN(PingRequestPacket, Status, Serverbound::Status::PingRequest)
		PACKET_FIELD_INT64(Timestamp)
	PACKET_DECL_END()

}