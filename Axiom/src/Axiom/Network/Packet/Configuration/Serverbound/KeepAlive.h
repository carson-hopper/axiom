#pragma once

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom::Configuration::Serverbound {

class KeepAlivePacket : public Packet<KeepAlivePacket,
    PID_CONFIGURATION_SB_CONFIGKEEPALIVE> {
public:
    std::optional<std::vector<Ref<IChainablePacket>>>
    Handle(const Ref<Connection> &connection, PacketContext& context) {
        AX_CORE_TRACE("Config keep-alive response from {}: {}",
            connection->RemoteAddress(), m_KeepAliveId.Value);

        return std::nullopt;
    }

    AX_START_FIELDS()
        AX_DECLARE(KeepAliveId)
    AX_END_FIELDS()

    AX_FIELD(KeepAliveId, int64_t);
};

} // namespace Axiom::Configuration::Serverbound
