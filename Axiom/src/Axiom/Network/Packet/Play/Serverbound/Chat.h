#pragma once

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom::Play::Serverbound {

class ChatPacket : public Packet<ChatPacket, PID_PLAY_SB_CHAT> {
public:
	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>& connection, PacketContext&, NetworkBuffer& buffer) {
		Parse(buffer);

		if (buffer.ReadBoolean()) {
			m_Signature.Value = buffer.ReadBytes(256);
		}

		AX_CORE_TRACE("Chat from {}: {}", connection->RemoteAddress(), m_Message.Value);
		AX_CORE_INFO("[Chat] {}", m_Message.Value);
		return std::nullopt;
	}


	AX_START_FIELDS()
		AX_DECLARE(Message),
		AX_DECLARE(Timestamp),
		AX_DECLARE(Salt)
	AX_END_FIELDS()

	AX_FIELD(Message, std::string)
	AX_FIELD(Timestamp, int64_t)
	AX_FIELD(Salt, int64_t)
	AX_FIELD(Signature, std::vector<uint8_t>)
};

} // namespace Axiom::Play::Serverbound
