#pragma once

#include "Axiom/Core/UUID.h"
#include "Axiom/Network/Packet/Packet.h"

namespace Axiom::Login::Clientbound {

class LoginFinishedPacket : public Packet<LoginFinishedPacket,
	PID_LOGIN_CB_LOGINFINISHED> {
public:
	LoginFinishedPacket() = default;

	LoginFinishedPacket(const UUID& uuid, std::string playerName) {
		m_Uuid.Value = uuid;
		m_PlayerName.Value = std::move(playerName);
	}

	LoginFinishedPacket(const std::string& uuidString, std::string playerName) {
		m_Uuid.Value = UUID::FromString(uuidString);
		m_PlayerName.Value = std::move(playerName);
	}

	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>&, PacketContext&, NetworkBuffer&) { return std::nullopt; }

	AX_START_FIELDS()
		AX_DECLARE(Uuid),
		AX_DECLARE(PlayerName),
		AX_DECLARE(PropertiesCount)
	AX_END_FIELDS()

	AX_FIELD(Uuid, Net::UuidType)
	AX_FIELD(PlayerName, Net::String)
	AX_FIELD_WITH_DEFAULT(PropertiesCount, Net::VarInt, 0)
};

} // namespace Axiom::Login::Clientbound
