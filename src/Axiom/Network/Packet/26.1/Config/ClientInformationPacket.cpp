#include "ClientInformationPacket.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom {

	template<int32_t Version>
	void ClientInformationPacket<Version>::Handle(Ref<Connection> connection, PacketContext& /*context*/) {
		AX_CORE_TRACE("Client info from {}: locale={}, viewDistance={}, mainHand={}",
			connection->RemoteAddress(), locale, viewDistance, mainHand);
	}

	template class ClientInformationPacket<775>;

}
