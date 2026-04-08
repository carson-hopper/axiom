#include "SystemChatPacket.h"

#include "Axiom/Network/NetworkBuffer.h"
#include "Axiom/Chat/ChatComponent.h"

namespace Axiom {

	CLIENTBOUND_PACKET_ENCODE_BEGIN(SystemChatPacket)
		// Write the chat component as JSON
		if (m_Message) {
			buffer.WriteString(m_Message->ToJson());
		} else {
			buffer.WriteString("{\"text\":\"\"}");
		}
		
		// Write action bar flag
		buffer.WriteBoolean(m_ActionBar);
	CLIENTBOUND_PACKET_ENCODE_END()

	CLIENTBOUND_PACKET_INSTANTIATE(SystemChatPacket, 775)

}
