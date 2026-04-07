#include "PingRequestPacket.h"

#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom {

	template<int32_t Version>
	void PingRequestPacket<Version>::Handle(const Ref<Connection> connection, PacketContext& /*context*/) {
		NetworkBuffer payload;
		payload.WriteLong(timestamp);
		connection->SendRawPacket(Clientbound::Status::PongResponse, payload);
	}

	template class PingRequestPacket<775>;

}
