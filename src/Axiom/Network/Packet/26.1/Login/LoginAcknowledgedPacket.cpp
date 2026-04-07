#include "LoginAcknowledgedPacket.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Protocol.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom {

PACKET_HANDLE_BEGIN(LoginAcknowledgedPacket)
    AX_CORE_TRACE("Login acknowledged from {}", connection->RemoteAddress());
    connection->State(ConnectionState::Configuration);

    // Send all registry data
    context.Registries().SendRegistries(connection);

    // Send all tags
    context.Registries().SendTags(connection);

    // Send FinishConfiguration
    {
        const NetworkBuffer payload;
        connection->SendRawPacket(Clientbound::Config::FinishConfiguration, payload);
    }
PACKET_HANDLE_END()

PACKET_INSTANTIATE(LoginAcknowledgedPacket, 775)

}
