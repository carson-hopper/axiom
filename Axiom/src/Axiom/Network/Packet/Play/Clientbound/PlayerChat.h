#pragma once

#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Core/UUID.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"

#include <optional>
#include <vector>

namespace Axiom::Play::Clientbound {

class PlayerChatPacket : public Packet<PlayerChatPacket, PID_PLAY_CB_PLAYERCHAT> {
public:
	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>&, PacketContext&, NetworkBuffer&) { return std::nullopt; }

	void Write(NetworkBuffer& buffer) override {
		buffer.WriteLong(static_cast<int64_t>(static_cast<uint64_t>(m_SenderUuid.Value)));
		buffer.WriteLong(0);

		if (m_Signature.Value.has_value()) {
			buffer.WriteBoolean(true);
			buffer.WriteVarInt(static_cast<int32_t>(m_Signature.Value->size()));
			buffer.WriteBytes(m_Signature.Value.value());
		} else {
			buffer.WriteBoolean(false);
		}

		buffer.WriteTextComponent(m_Message.Value->ToJson());
		buffer.WriteLong(m_Timestamp.Value);
		buffer.WriteLong(m_Salt.Value);

		buffer.WriteVarInt(static_cast<int32_t>(m_PreviousMessages.Value.size()));
		for (int32_t previousMessage : m_PreviousMessages.Value) {
			buffer.WriteVarInt(previousMessage);
		}

		if (m_UnsignedContent.Value.has_value()) {
			buffer.WriteBoolean(true);
			buffer.WriteTextComponent(m_UnsignedContent.Value.value()->ToJson());
		} else {
			buffer.WriteBoolean(false);
		}

		buffer.WriteVarInt(m_FilterType.Value);
		buffer.WriteVarInt(m_ChatType.Value);
		buffer.WriteString(m_SenderName.Value);

		if (m_TargetName.Value.has_value()) {
			buffer.WriteBoolean(true);
			buffer.WriteString(m_TargetName.Value.value());
		} else {
			buffer.WriteBoolean(false);
		}
	}

    AX_START_FIELDS()
        AX_DECLARE(SenderUuid),
        AX_DECLARE(Signature),
        AX_DECLARE(Message),
        AX_DECLARE(Timestamp),
        AX_DECLARE(Salt),
        AX_DECLARE(PreviousMessages),
        AX_DECLARE(UnsignedContent),
        AX_DECLARE(FilterType),
        AX_DECLARE(ChatType),
        AX_DECLARE(SenderName),
        AX_DECLARE(TargetName)
    AX_END_FIELDS()
        
	AX_FIELD(SenderUuid, UUID)
	AX_FIELD(Signature, std::optional<std::vector<uint8_t>>)
	AX_FIELD(Message, Ref<ChatComponent>)
	AX_FIELD(Timestamp, int64_t)
	AX_FIELD(Salt, int64_t)
	AX_FIELD(PreviousMessages, std::vector<int32_t>)
	AX_FIELD(UnsignedContent, std::optional<Ref<ChatComponent>>)
	AX_FIELD(FilterType, int32_t)
	AX_FIELD(ChatType, int32_t)
	AX_FIELD(SenderName, std::string)
	AX_FIELD(TargetName, std::optional<std::string>)
};

} // namespace Axiom::Play::Clientbound
