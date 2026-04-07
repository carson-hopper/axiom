#pragma once

#include "Axiom/Network/Packet/ServerboundPacket.h"
#include "Axiom/Network/Protocol.h"

#include <cstdint>

namespace Axiom {

	template<int32_t Version = PROTOCOL_VERSION>
	class ConfirmTeleportationPacket : public ServerboundPacket {
	public:
		static constexpr int32_t PacketId = Serverbound::Play::AcceptTeleportation;
		static constexpr auto PacketState = ConnectionState::Play;

		void Decode(NetworkBuffer& buffer) override {
			m_TeleportId = buffer.ReadVarInt();
		}

		void Handle(Ref<Connection> connection, PacketContext& context) override;

	private:
		int32_t m_TeleportId = 0;
	};

}
