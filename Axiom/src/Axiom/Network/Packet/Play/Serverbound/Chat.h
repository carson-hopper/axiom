#pragma once

#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"
#include "Axiom/Network/Packet/Play/Clientbound/SystemChat.h"

namespace Axiom::Play::Serverbound {

class ChatPacket : public Packet<ChatPacket, PID_PLAY_SB_CHAT> {
public:
	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>& connection, PacketContext& context, NetworkBuffer& buffer) {
		// Signature is variable-length and conditional — read manually after auto-parsed fields
		const int32_t signatureLength = buffer.ReadVarInt();
		if (signatureLength > 0) {
			m_Signature.Value = buffer.ReadBytes(signatureLength);
		}

		// MessageCount + acknowledged bitset
		m_Count.Value.Read(buffer);
		buffer.ReadFixedBitSet(20);
		buffer.ReadByte();
        
		const Ref<Player> sender = context.Server().GetPlayer(connection);
		std::string senderName = sender ? sender->Name() : "unknown";

		// Build and broadcast the chat message to all players
		const auto chatMessage = ChatComponent::Create()
			.Text("<" + senderName + "> " + m_Message.Value)
			.Build();
        
		Clientbound::SystemChatPacket chatPacket(chatMessage, false);
		context.Server().BroadcastPacket(chatPacket);

		AX_CORE_TRACE("Chat from {}: {}", connection->RemoteAddress(), m_Message.Value);
		AX_CORE_INFO("[Chat] <{}>: {}", senderName, m_Message.Value);
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
    AX_FIELD(Count, Net::VarInt)
};

} // namespace Axiom::Play::Serverbound
