#pragma once

#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom::Play::Clientbound {

class SystemChatPacket : public Packet<SystemChatPacket, PID_PLAY_CB_SYSTEMCHAT> {
public:
	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>&, PacketContext&, NetworkBuffer&) { return std::nullopt; }

	void Write(NetworkBuffer& buffer) override {
		if (m_Message.Value) {
			buffer.WriteTextComponent(m_Message.Value->ToJson());
		} else {
			buffer.WriteTextComponent("{\"text\":\"\"}");
		}
		buffer.WriteBoolean(m_ActionBar.Value);
	}

    AX_START_FIELDS()
    AX_END_FIELDS()

	AX_FIELD(Message, Ref<ChatComponent>)
	AX_FIELD(ActionBar, bool)
};

} // namespace Axiom::Play::Clientbound
