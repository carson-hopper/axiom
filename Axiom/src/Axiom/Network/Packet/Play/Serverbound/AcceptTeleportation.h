#pragma once

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom::Play::Serverbound {

class AcceptTeleportationPacket : public Packet<AcceptTeleportationPacket,
    PID_PLAY_SB_ACCEPTTELEPORTATION> {
public:
    std::optional<std::vector<Ref<IChainablePacket>>>
    Handle(const Ref<Connection> &connection, PacketContext& /*context*/) {
        AX_CORE_TRACE("Teleport accepted from {}: id={}",
            connection->RemoteAddress(), m_TeleportId.Value);

        return std::nullopt;
    }

    AX_START_FIELDS()
        AX_DECLARE(TeleportId)
    AX_END_FIELDS()

    AX_FIELD(TeleportId, int32_t);
};

} // namespace Axiom::Play::Serverbound
