#pragma once

#include "Axiom/Network/Packet/ServerboundPacket.h"
#include "Axiom/Network/Protocol.h"

#include <cstdint>

namespace Axiom {

	template<int32_t Version = PROTOCOL_VERSION>
	class ConfigKeepAlivePacket : public ServerboundPacket {
	public:
		static constexpr int32_t PacketId = Serverbound::Config::KeepAlive;
		static constexpr ConnectionState PacketState = ConnectionState::Configuration;

		void Decode(NetworkBuffer& buffer) override {
			keepAliveId = buffer.ReadLong();
		}

		void Handle(Ref<Connection> connection, PacketContext& context) override;

		int64_t keepAliveId = 0;
	};

}
