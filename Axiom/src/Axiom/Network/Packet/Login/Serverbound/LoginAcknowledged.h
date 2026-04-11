#pragma once

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Protocol.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"
#include "Axiom/Network/Packet/Configuration/Clientbound/SelectKnownPacks.h"

namespace Axiom::Login::Serverbound {

class LoginAcknowledgedPacket : public Packet<LoginAcknowledgedPacket,
	PID_LOGIN_SB_LOGINACKNOWLEDGED> {
public:
	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>& connection, PacketContext&, NetworkBuffer&) {
		AX_CORE_TRACE("Login acknowledged from {}", connection->RemoteAddress());
		connection->State(ConnectionState::Configuration);

		// Vanilla 1.21+ requires the server to advertise its known
		// packs before any RegistryData/UpdateTags. The client echoes
		// back the intersection in ServerboundSelectKnownPacks, which
		// our Configuration::Serverbound::SelectKnownPacksPacket
		// handler catches and uses to drive the rest of the config
		// phase (registries + tags + finish-configuration).
		return CreateChainPackets<Configuration::Clientbound::SelectKnownPacksPacket>();
	}

	AX_START_FIELDS()
	AX_END_FIELDS()
};

} // namespace Axiom::Login::Serverbound
