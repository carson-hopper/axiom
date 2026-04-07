#include "KeepAlivePacket.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom {

	template<int32_t Version>
	void ConfigKeepAlivePacket<Version>::Handle(const Ref<Connection> connection, PacketContext& /*context*/) {
		AX_CORE_TRACE("Config keep-alive response from {}: {}", connection->RemoteAddress(), keepAliveId);
	}

	template class ConfigKeepAlivePacket<775>;

}
