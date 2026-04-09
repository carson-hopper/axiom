#pragma once

#include "Axiom/Network/Packet/Packet.h"

namespace Axiom::Status::Clientbound {

class StatusResponsePacket : public Packet<StatusResponsePacket,
	PID_STATUS_CB_STATUSRESPONSE> {
public:
	StatusResponsePacket() = default;
	StatusResponsePacket(std::string json) { m_Json.Value = std::move(json); }

	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>&, PacketContext&, NetworkBuffer&) { return std::nullopt; }

	void Write(NetworkBuffer& buffer) override {
		buffer.WriteString(m_Json.Value);
	}

	AX_START_FIELDS()
		AX_DECLARE(Json)
	AX_END_FIELDS()

	AX_FIELD(Json, std::string)
};

} // namespace Axiom::Status::Clientbound
