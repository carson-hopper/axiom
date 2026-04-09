#pragma once

#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom::Status::Serverbound {

class PingRequestPacket : public Packet<PingRequestPacket,
    PID_STATUS_SB_PINGREQUEST> {
public:
    std::optional<std::vector<Ref<IChainablePacket>>>
    Handle(const Ref<Connection>& connection, const PacketContext& /*context*/) const {
        NetworkBuffer payload;
        payload.WriteLong(m_Payload.Value);
        connection->SendRawPacket(Clientbound::Status::PongResponse, payload);

        return std::nullopt;
    }

    AX_START_FIELDS()
        AX_DECLARE(Payload)
    AX_END_FIELDS()

    AX_FIELD(Payload, int64_t);
};

} // namespace Axiom::Status::Serverbound
