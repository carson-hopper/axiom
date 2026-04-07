#include "MovePlayerPositionPacket.h"

#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom {

	// ----- MovePlayerPosition ---------------------------------------

	PACKET_DECODE_BEGIN(MovePlayerPositionPacket)
		m_Position = buffer.ReadVector3();
		READ_BYTE(m_Flags);
	PACKET_DECODE_END()

	PACKET_HANDLE_BEGIN(MovePlayerPositionPacket)
		context.ChunkManagement().OnPlayerMove(connection, m_Position.x, m_Position.z);
	PACKET_HANDLE_END()

	PACKET_INSTANTIATE(MovePlayerPositionPacket, 775)

	// ----- MovePlayerPositionRotation -------------------------------

	PACKET_DECODE_BEGIN(MovePlayerPositionRotationPacket)
		m_Position = buffer.ReadVector3();
		m_Rotation = buffer.ReadVector2();
		READ_BYTE(m_Flags);
	PACKET_DECODE_END()

	PACKET_HANDLE_BEGIN(MovePlayerPositionRotationPacket)
		context.ChunkManagement().OnPlayerMove(connection, m_Position.x, m_Position.z);
	PACKET_HANDLE_END()

	PACKET_INSTANTIATE(MovePlayerPositionRotationPacket, 775)

	// ----- MovePlayerRotation ---------------------------------------

	PACKET_DECODE_BEGIN(MovePlayerRotationPacket)
		m_Rotation = buffer.ReadVector2();
		READ_BYTE(m_Flags);
	PACKET_DECODE_END()

	PACKET_HANDLE_BEGIN(MovePlayerRotationPacket)
		// Rotation only - no chunk updates needed
	PACKET_HANDLE_END()

	PACKET_INSTANTIATE(MovePlayerRotationPacket, 775)

	// ----- MovePlayerStatusOnly -------------------------------------

	PACKET_DECODE_BEGIN(MovePlayerStatusOnlyPacket)
		READ_BYTE(m_Flags);
	PACKET_DECODE_END()

	PACKET_HANDLE_BEGIN(MovePlayerStatusOnlyPacket)
		// Status only - no position data
	PACKET_HANDLE_END()

	PACKET_INSTANTIATE(MovePlayerStatusOnlyPacket, 775)

}