#pragma once

#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/Packet.h"

namespace Axiom::Login::Clientbound {

class LoginCompressionPacket : public Packet<LoginCompressionPacket,
	PID_LOGIN_CB_LOGINCOMPRESSION> {
public:
	LoginCompressionPacket() = default;
	explicit LoginCompressionPacket(int32_t threshold) { m_Threshold.Value = threshold; }

	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>&, PacketContext&, NetworkBuffer&) { return std::nullopt; }

	void OnSent(const Ref<Connection>& connection) override {
		connection->SetCompressionThreshold(m_Threshold.Value.GetValue());
	}

	AX_START_FIELDS()
		AX_DECLARE(Threshold)
	AX_END_FIELDS()

	AX_FIELD_WITH_DEFAULT(Threshold, Net::VarInt, 256)
};

} // namespace Axiom::Login::Clientbound
