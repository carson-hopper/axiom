#pragma once

/**
 * @file EncryptionResponsePacket.h
 *
 * Client sends encrypted shared secret and verify token during login.
 */

#include "Axiom/Network/Packet/PacketVersioned.h"

#include <vector>

namespace Axiom {

PACKET_VERSIONED(EncryptionResponsePacket, 775, Serverbound, Login, 0x01)
	FIELD_ARRAY(uint8_t, EncryptedSharedSecret)
	FIELD_ARRAY(uint8_t, EncryptedVerifyToken)
PACKET_VERSIONED_END(EncryptionResponsePacket, 775)

}
