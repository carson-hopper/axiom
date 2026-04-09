#pragma once

#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/Status/Clientbound/PongResponse.h"

namespace Axiom::Status::Serverbound {

class PingRequestPacket : public Packet<PingRequestPacket,
	PID_STATUS_SB_PINGREQUEST> {
public:
	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>&, PacketContext&, NetworkBuffer&) {
		return CreateChainPacketsWithArgs<Clientbound::PongResponsePacket>(
			std::make_tuple(m_Payload.Value));
	}

	AX_START_FIELDS()
		AX_DECLARE(Payload)
	AX_END_FIELDS()

	AX_FIELD(Payload, int64_t)
};

} // namespace Axiom::Status::Serverbound
