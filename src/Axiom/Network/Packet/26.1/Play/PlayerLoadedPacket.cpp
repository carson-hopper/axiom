#include "PlayerLoadedPacket.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom {

PACKET_HANDLE_BEGIN(PlayerLoadedPacket)
    AX_CORE_INFO("Player fully loaded from {}", connection->RemoteAddress());
PACKET_HANDLE_END()

PACKET_INSTANTIATE(PlayerLoadedPacket, 775)

}
