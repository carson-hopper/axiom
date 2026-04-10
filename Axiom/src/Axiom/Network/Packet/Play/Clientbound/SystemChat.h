#pragma once

#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Data/Nbt/NbtCompound.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom::Play::Clientbound {

class SystemChatPacket : public Packet<SystemChatPacket, PID_PLAY_CB_SYSTEMCHAT> {
public:
	SystemChatPacket() = default;

	SystemChatPacket(const Ref<ChatComponent>& component, const bool actionBar) {
		m_Message.Value = component ? component->ToNbt() : CreateRef<NbtCompound>();
		m_ActionBar.Value = actionBar;
	}

	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>&, PacketContext&, NetworkBuffer&) { return std::nullopt; }

	AX_START_FIELDS()
		AX_DECLARE(Message),
		AX_DECLARE(ActionBar)
	AX_END_FIELDS()

	AX_FIELD(Message, Ref<NbtCompound>)
	AX_FIELD(ActionBar, bool)
};

} // namespace Axiom::Play::Clientbound
