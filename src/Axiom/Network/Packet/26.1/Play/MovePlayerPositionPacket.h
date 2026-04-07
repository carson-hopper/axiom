#pragma once

/**
 * @file MovePlayerPositionPacket.h
 * @brief Player position and rotation update packets.
 */

#include "Axiom/Network/Packet/PacketMacros.h"

namespace Axiom {

// ----- Position Only --------------------------------------------

PACKET_DECL_BEGIN(MovePlayerPositionPacket, Play, Serverbound::Play::MovePlayerPosition)
	PACKET_FIELD(Vector3, Position, {})
	PACKET_FIELD_UINT8(Flags)
PACKET_DECL_END()

// ----- Position and Rotation ------------------------------------

PACKET_DECL_BEGIN(MovePlayerPositionRotationPacket, Play, Serverbound::Play::MovePlayerPositionRotation)
	PACKET_FIELD(Vector3, Position, {})
	PACKET_FIELD(Vector2, Rotation, {})
	PACKET_FIELD_UINT8(Flags)
PACKET_DECL_END()

// ----- Rotation Only --------------------------------------------

PACKET_DECL_BEGIN(MovePlayerRotationPacket, Play, Serverbound::Play::MovePlayerRotation)
	PACKET_FIELD(Vector2, Rotation, {})
	PACKET_FIELD_UINT8(Flags)
PACKET_DECL_END()

// ----- Status Only (On Ground) ----------------------------------

PACKET_DECL_BEGIN(MovePlayerStatusOnlyPacket, Play, Serverbound::Play::MovePlayerStatusOnly)
	PACKET_FIELD_UINT8(Flags)
PACKET_DECL_END()

}