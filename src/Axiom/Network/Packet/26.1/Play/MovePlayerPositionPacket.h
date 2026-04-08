#pragma once

#include "Axiom/Network/Packet/PacketVersioned.h"

namespace Axiom {

PACKET_VERSIONED_SB(MovePlayerPositionPacket, 775, Play, Serverbound::Play::MovePlayerPosition)
	FIELD(Vector3, Position, {})
	FIELD_BYTE_ARRAY(1, OnGround)
PACKET_VERSIONED_END(MovePlayerPositionPacket, 775)

PACKET_VERSIONED_SB(MovePlayerPositionRotationPacket, 775, Play, Serverbound::Play::MovePlayerPositionRotation)
	FIELD(Vector3, Position, {})
	FIELD(Vector2, Rotation, {})
	FIELD_BYTE_ARRAY(1, OnGround)
PACKET_VERSIONED_END(MovePlayerPositionRotationPacket, 775)

PACKET_VERSIONED_SB(MovePlayerRotationPacket, 775, Play, Serverbound::Play::MovePlayerRotation)
	FIELD(Vector2, Rotation, {})
	FIELD_BYTE_ARRAY(1, OnGround)
PACKET_VERSIONED_END(MovePlayerRotationPacket, 775)

PACKET_VERSIONED_SB(MovePlayerStatusOnlyPacket, 775, Play, Serverbound::Play::MovePlayerStatusOnly)
	FIELD_BYTE_ARRAY(1, OnGround)
PACKET_VERSIONED_END(MovePlayerStatusOnlyPacket, 775)

}
