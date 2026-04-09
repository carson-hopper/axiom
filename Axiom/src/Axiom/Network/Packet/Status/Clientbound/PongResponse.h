#pragma once

#include "Axiom/Network/Packet/Packet.h"

namespace Axiom::Status::Clientbound {

class PongResponsePacket : public Packet<PongResponsePacket,
	PID_STATUS_CB_PONGRESPONSE> {
public:
	PongResponsePacket() = default;
	PongResponsePacket(int64_t payload) { m_Payload.Value = payload; }

	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>&, PacketContext&, NetworkBuffer&) { return std::nullopt; }

	void Write(NetworkBuffer& buffer) override {
		buffer.WriteLong(m_Payload.Value);
	}

	AX_START_FIELDS()
		AX_DECLARE(Payload)
	AX_END_FIELDS()

	AX_FIELD(Payload, int64_t)
};

} // namespace Axiom::Status::Clientbound
