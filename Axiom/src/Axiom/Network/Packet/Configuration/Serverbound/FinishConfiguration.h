#pragma once

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/PlayStateBootstrap.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom::Configuration::Serverbound {

class FinishConfigurationPacket : public Packet<FinishConfigurationPacket,
    PID_CONFIGURATION_SB_FINISHCONFIGURATION> {
public:
    std::optional<std::vector<Ref<IChainablePacket>>>
    Handle(const Ref<Connection> &connection, PacketContext& context) {
        AX_CORE_INFO("Configuration complete for {}",
            connection->RemoteAddress());
        connection->State(ConnectionState::Play);

        PlayStateBootstrap::Bootstrap(connection, context);

        return std::nullopt;
    }

	AX_START_FIELDS()

	AX_END_FIELDS()
};

} // namespace Axiom::Configuration::Serverbound
