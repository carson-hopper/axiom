#pragma once

#include "Axiom/Network/Packet/ServerboundPacket.h"
#include "Axiom/Network/Protocol.h"

#include <cstdint>

namespace Axiom {

	template<int32_t Version = PROTOCOL_VERSION>
	class PingRequestPacket : public ServerboundPacket {
	public:
		static constexpr int32_t PacketId = Serverbound::Status::PingRequest;
		static constexpr ConnectionState PacketState = ConnectionState::Status;

		void Decode(NetworkBuffer& buffer) override {
			timestamp = buffer.ReadLong();
		}

		void Handle(Ref<Connection> connection, PacketContext& context) override;

		int64_t timestamp = 0;
	};

}
