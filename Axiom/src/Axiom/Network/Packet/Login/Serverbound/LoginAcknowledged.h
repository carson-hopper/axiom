#pragma once

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Protocol.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"
#include "Axiom/Network/Packet/Configuration/Clientbound/FinishConfiguration.h"

namespace Axiom::Login::Serverbound {

class LoginAcknowledgedPacket : public Packet<LoginAcknowledgedPacket,
	PID_LOGIN_SB_LOGINACKNOWLEDGED> {
public:
	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>& connection, PacketContext& context, NetworkBuffer&) {
		AX_CORE_TRACE("Login acknowledged from {}", connection->RemoteAddress());
		connection->State(ConnectionState::Configuration);

		// RegistryDataService handles the complex multi-packet registry sync
		context.Registries().SendRegistries(connection);
		context.Registries().SendTags(connection);

		return CreateChainPackets<Configuration::Clientbound::FinishConfigurationPacket>();
	}

	AX_START_FIELDS()
	AX_END_FIELDS()
};

} // namespace Axiom::Login::Serverbound
