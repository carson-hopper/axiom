#include "FinishConfigurationPacket.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom {

	template<int32_t Version>
	void FinishConfigurationPacket<Version>::Handle(Ref<Connection> connection, PacketContext& /*context*/) {
		AX_CORE_INFO("Configuration complete for {}", connection->RemoteAddress());
		connection->State(ConnectionState::Play);
	}

	template class FinishConfigurationPacket<775>;

}
