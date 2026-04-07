#pragma once

#include "Axiom/Network/Packet/ServerboundPacket.h"
#include "Axiom/Network/Protocol.h"

#include <cstdint>

namespace Axiom {

	template<int32_t Version = PROTOCOL_VERSION>
	class AcceptTeleportationPacket : public ServerboundPacket {
	public:
		static constexpr int32_t PacketId = Serverbound::Play::AcceptTeleportation;
		static constexpr ConnectionState PacketState = ConnectionState::Play;

		void Decode(NetworkBuffer& buffer) override {
			teleportId = buffer.ReadVarInt();
		}

		void Handle(Ref<Connection> connection, PacketContext& context) override;

		int32_t teleportId = 0;
	};

}
