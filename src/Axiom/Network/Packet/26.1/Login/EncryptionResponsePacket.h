#pragma once

/**
 * @file EncryptionResponsePacket.h
 *
 * Client sends encrypted shared secret and verify token during login.
 */

#include "Axiom/Network/Packet/PacketMacros.h"

#include <vector>

namespace Axiom {

PACKET_DECL_BEGIN(EncryptionResponsePacket, Login, Serverbound::Login::Key)
	PACKET_FIELD(std::vector<uint8_t>, EncryptedSharedSecret, {})
	PACKET_FIELD(std::vector<uint8_t>, EncryptedVerifyToken, {})
PACKET_DECL_END()

}