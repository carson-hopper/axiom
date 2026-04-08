#include "PlayerChatPacket.h"

#include "Axiom/Network/NetworkBuffer.h"
#include "Axiom/Chat/ChatComponent.h"

namespace Axiom {

	CLIENTBOUND_PACKET_ENCODE_BEGIN(PlayerChatPacket)
		// Write sender UUID (16 bytes)
		// Parse UUID string and write as bytes
		{
			std::vector<uint8_t> uuidBytes;
			uuidBytes.reserve(16);
			
			// Simple UUID parser - remove dashes and convert hex to bytes
			std::string cleanUUID;
			for (char c : m_SenderUUID) {
				if (c != '-') {
					cleanUUID += c;
				}
			}
			
			if (cleanUUID.size() == 32) {
				for (size_t i = 0; i < 32; i += 2) {
					std::string byteStr = cleanUUID.substr(i, 2);
					uint8_t byte = static_cast<uint8_t>(std::stoi(byteStr, nullptr, 16));
					uuidBytes.push_back(byte);
				}
			} else {
				// Fill with zeros if invalid
				uuidBytes.resize(16, 0);
			}
			
			buffer.WriteBytes(uuidBytes);
		}
		
		// Write signature index (for chat chain validation)
		// -1 means no previous signature
		buffer.WriteVarInt(-1);
		
		// Write message body
		if (m_Message) {
			buffer.WriteString(m_Message->ToJson());
		} else {
			buffer.WriteString("{\"text\":\"\"}");
		}
		
		// Write timestamp
		buffer.WriteLong(m_Timestamp);
		
		// Write salt (0 for now - not using signed chat)
		buffer.WriteLong(0);
		
		// Write previous message count (0 for now)
		buffer.WriteVarInt(0);
		
		// Write signature present flag
		buffer.WriteBoolean(m_Signed);
		
		// Write signature if present
		if (m_Signed && m_Signature.size() == 256) {
			buffer.WriteBytes(m_Signature);
		}
		
		// Write filter type (0 = not filtered)
		buffer.WriteVarInt(0);
		
		// Write chat type (1 = chat)
		buffer.WriteVarInt(1);
		
		// Write sender name
		buffer.WriteString(m_SenderName);
		
		// Write target name (empty for regular chat)
		buffer.WriteBoolean(false); // No target name
	CLIENTBOUND_PACKET_ENCODE_END()

	CLIENTBOUND_PACKET_INSTANTIATE(PlayerChatPacket, 775)

}
