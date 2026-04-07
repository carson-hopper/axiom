#include "LoginAcknowledgedPacket.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Protocol.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom {

	template<int32_t Version>
	void LoginAcknowledgedPacket<Version>::Handle(Ref<Connection> connection, PacketContext& context) {
		AX_CORE_TRACE("Login acknowledged from {}", connection->RemoteAddress());
		connection->State(ConnectionState::Configuration);

		// Send all registry data
		context.Registries().SendRegistries(connection);

		// Send all tags
		context.Registries().SendTags(connection);

		// Send FinishConfiguration
		{
			NetworkBuffer payload;
			connection->SendRawPacket(Clientbound::Config::FinishConfiguration, payload);
		}
	}

	template class LoginAcknowledgedPacket<775>;

}
