#pragma once

#include "Axiom/Network/Packet/ServerboundPacket.h"
#include "Axiom/Network/Protocol.h"

namespace Axiom {

	/**
	 * Client tells the server which hotbar slot is selected (0-8).
	 */
	template<int32_t Version = PROTOCOL_VERSION>
	class SetCarriedItemPacket : public ServerboundPacket {
	public:
		static constexpr int32_t PacketId = Serverbound::Play::SetCarriedItem;
		static constexpr ConnectionState PacketState = ConnectionState::Play;

		void Decode(NetworkBuffer& buffer) override {
			m_Slot = buffer.ReadShort();
		}

		void Handle(Ref<Connection> connection, PacketContext& context) override;

	private:
		int16_t m_Slot = 0;
	};

}
