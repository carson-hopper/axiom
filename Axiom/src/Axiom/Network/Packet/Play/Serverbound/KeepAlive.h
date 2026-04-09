#pragma once

#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom::Play::Serverbound {

class KeepAlivePacket : public Packet<KeepAlivePacket,
    PID_PLAY_SB_PLAYKEEPALIVE> {
public:
    std::optional<std::vector<Ref<IChainablePacket>>>
    Handle(const Ref<Connection> &connection, PacketContext& context, NetworkBuffer&) {
        context.KeepAlive().OnKeepAliveResponse(connection->Id(), m_KeepAliveId.Value);

        return std::nullopt;
    }

    AX_START_FIELDS()
        AX_DECLARE(KeepAliveId)
    AX_END_FIELDS()

    AX_FIELD(KeepAliveId, int64_t);
};

} // namespace Axiom::Play::Serverbound
