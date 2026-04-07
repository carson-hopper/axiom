#pragma once

/**
 * @file LoginHelloPacket.h
 * @brief Login start packet sent by the client.
 */

#include "Axiom/Network/Packet/PacketMacros.h"

#include <string>

namespace Axiom {

	PACKET_DECL_BEGIN(LoginHelloPacket, Login, Serverbound::Login::Hello)
		PACKET_FIELD_STRING(PlayerName)
	PACKET_DECL_END()

}