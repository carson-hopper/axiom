#include "PlayKeepAlivePacket.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom {

	template<int32_t Version>
	void PlayKeepAlivePacket<Version>::Handle(Ref<Connection> connection, PacketContext& /*context*/) {
		AX_CORE_TRACE("Keep-alive response from {}: {}", connection->RemoteAddress(), keepAliveId);
	}

	template class PlayKeepAlivePacket<775>;

}
