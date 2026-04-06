#pragma once

#include "Axiom/Network/Packet/ServerboundPacket.h"
#include "Axiom/Network/Protocol.h"

#include <string>

namespace Axiom {

	template<int32_t Version = PROTOCOL_VERSION>
	class LoginHelloPacket : public ServerboundPacket {
	public:
		static constexpr int32_t PacketId = Serverbound::Login::Hello;
		static constexpr ConnectionState PacketState = ConnectionState::Login;

		void Decode(NetworkBuffer& buffer) override {
			playerName = buffer.ReadString(16);
		}

		void Handle(Ref<Connection> connection, PacketContext& context) override;

		std::string playerName;
	};

}
