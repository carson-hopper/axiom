#include "PlayerLoadedPacket.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom {

	template<int32_t Version>
	void PlayerLoadedPacket<Version>::Handle(const Ref<Connection> connection, PacketContext& /*context*/) {
		AX_CORE_INFO("Player fully loaded from {}", connection->RemoteAddress());
	}

	template class PlayerLoadedPacket<775>;

}
