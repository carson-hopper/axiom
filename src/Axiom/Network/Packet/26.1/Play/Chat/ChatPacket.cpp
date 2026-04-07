#include "axpch.h"
#include "ChatPacket.h"

#include "Axiom/Network/Connection.h"

namespace Axiom {

	PACKET_DECODE_BEGIN(ChatPacket)
		READ_STRING(m_Message);
		READ_INT64(m_Timestamp);
		READ_INT64(m_Salt);

		// Check if message is signed
		if (bool hasSignature = buffer.ReadBoolean()) {
			// Read signature (256 bytes for signed messages)
			m_Signature = buffer.ReadBytes(256);
		}
	PACKET_DECODE_END()

	PACKET_HANDLE_BEGIN(ChatPacket)
		AX_CORE_TRACE("Chat from {}: {}", connection->RemoteAddress(), m_Message);

		// TODO: Process chat message
		// 1. Validate message (anti-spam, profanity filter, etc.)
		// 2. Log to console/log file
		// 3. Broadcast to other players
		// 4. Trigger chat events for plugins

		// For now, just echo to console
		AX_CORE_INFO("[Chat] {}", m_Message);
	PACKET_HANDLE_END()

	PACKET_INSTANTIATE(ChatPacket, 775)

}
