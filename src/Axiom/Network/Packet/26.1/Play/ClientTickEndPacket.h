#pragma once

#include "Axiom/Network/Packet/ServerboundPacket.h"
#include "Axiom/Network/Protocol.h"

namespace Axiom {

	template<int32_t Version = PROTOCOL_VERSION>
	class ClientTickEndPacket : public ServerboundPacket {
	public:
		static constexpr int32_t PacketId = Serverbound::Play::ClientTickEnd;
		static constexpr ConnectionState PacketState = ConnectionState::Play;

		void Decode(NetworkBuffer& /*buffer*/) override {}
		void Handle(Ref<Connection> connection, PacketContext& context) override;
	};

}
