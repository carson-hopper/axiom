#include "ClientTickEndPacket.h"

#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom {

	template<int32_t Version>
	void ClientTickEndPacket<Version>::Handle(const Ref<Connection> /*connection*/, PacketContext& /*context*/) {
		// No-op — client signals end of tick processing
	}

	template class ClientTickEndPacket<775>;

}
