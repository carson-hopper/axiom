#pragma once

#include "Axiom/Network/Packet/Packet.h"

namespace Axiom::Configuration::Clientbound {

class SelectKnownPacksPacket : public Packet<SelectKnownPacksPacket,
	PID_CONFIGURATION_CB_SELECTKNOWNPACKS> {
public:
	SelectKnownPacksPacket() = default;

	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>& /* connection */, const PacketContext& /* context */, const NetworkBuffer& /* buffer */) {
		return std::nullopt;
	}

	void Write(NetworkBuffer& buffer) override {
		buffer.WriteVarInt(1);
		buffer.WriteString("minecraft");
		buffer.WriteString("core");
		buffer.WriteString(AX_MINECRAFT_VERSION);
	}

	AX_START_FIELDS()
	AX_END_FIELDS()
};

} // namespace Axiom::Configuration::Clientbound
