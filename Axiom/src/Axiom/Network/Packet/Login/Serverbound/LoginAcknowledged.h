#pragma once

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Protocol.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom::Login::Serverbound {

class LoginAcknowledgedPacket : public Packet<LoginAcknowledgedPacket,
    PID_LOGIN_SB_LOGINACKNOWLEDGED> {
public:
    std::optional<std::vector<Ref<IChainablePacket>>>
    Handle(const Ref<Connection> &connection, PacketContext& context) {
        AX_CORE_TRACE("Login acknowledged from {}",
            connection->RemoteAddress());
        connection->State(ConnectionState::Configuration);

        context.Registries().SendRegistries(connection);
        context.Registries().SendTags(connection);

        {
            const NetworkBuffer payload;
            connection->SendRawPacket(
                Clientbound::Config::FinishConfiguration, payload);
        }

        return std::nullopt;
    }

	AX_START_FIELDS()

	AX_END_FIELDS()
};

} // namespace Axiom::Login::Serverbound
