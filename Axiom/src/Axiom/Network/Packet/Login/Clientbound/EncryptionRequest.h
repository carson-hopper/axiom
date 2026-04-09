#pragma once

#include "Axiom/Network/Packet/Packet.h"

namespace Axiom::Login::Clientbound {

class EncryptionRequestPacket : public Packet<EncryptionRequestPacket,
	PID_LOGIN_CB_HELLO> {
public:
	EncryptionRequestPacket() = default;
	EncryptionRequestPacket(std::string serverId,
		std::vector<uint8_t> publicKey,
		std::vector<uint8_t> verifyToken,
		bool shouldAuthenticate) {
		m_ServerId.Value = std::move(serverId);
		m_PublicKey.Value = std::move(publicKey);
		m_VerifyToken.Value = std::move(verifyToken);
		m_ShouldAuthenticate.Value = shouldAuthenticate;
	}

	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>&, PacketContext&, NetworkBuffer&) { return std::nullopt; }

	AX_START_FIELDS()
		AX_DECLARE(ServerId),
		AX_DECLARE(PublicKey),
		AX_DECLARE(VerifyToken),
		AX_DECLARE(ShouldAuthenticate)
	AX_END_FIELDS()

	AX_FIELD(ServerId, Net::String)
	AX_FIELD(PublicKey, Net::ByteArray)
	AX_FIELD(VerifyToken, Net::ByteArray)
	AX_FIELD_WITH_DEFAULT(ShouldAuthenticate, Net::Bool, true)
};

} // namespace Axiom::Login::Clientbound
