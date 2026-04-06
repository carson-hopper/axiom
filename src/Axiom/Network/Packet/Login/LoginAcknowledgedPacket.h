#pragma once

#include "Axiom/Network/Packet/ServerboundPacket.h"
#include "Axiom/Network/Protocol.h"

namespace Axiom {

	template<int32_t Version = PROTOCOL_VERSION>
	class LoginAcknowledgedPacket : public ServerboundPacket {
	public:
		static constexpr int32_t PacketId = Serverbound::Login::LoginAcknowledged;
		static constexpr ConnectionState PacketState = ConnectionState::Login;

		void Decode(NetworkBuffer& /*buffer*/) override {}
		void Handle(Ref<Connection> connection, PacketContext& context) override;
	};

}
