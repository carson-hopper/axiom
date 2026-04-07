#include "LoginAcknowledgedPacket.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom {

	template<int32_t Version>
	void LoginAcknowledgedPacket<Version>::Handle(Ref<Connection> connection, PacketContext& /*context*/) {
		AX_CORE_TRACE("Login acknowledged from {}", connection->RemoteAddress());
		connection->State(ConnectionState::Configuration);

		// Send SelectKnownPacks to client (server offers its known packs)
		{
			NetworkBuffer payload;
			payload.WriteVarInt(1);  // 1 known pack
			payload.WriteString("minecraft");  // namespace
			payload.WriteString("core");       // id
			payload.WriteString("26.1");       // version
			connection->SendRawPacket(Clientbound::Config::SelectKnownPacks, payload);
		}
	}

	template class LoginAcknowledgedPacket<775>;

}
