#pragma once

#include "Axiom/Network/Packet/ServerboundPacket.h"
#include "Axiom/Network/Protocol.h"

#include <cstdint>

namespace Axiom {

	/**
	 * Player Action: digging (block breaking), drop item, swap hands, etc.
	 * Action 0 = Started digging, 1 = Cancelled, 2 = Finished digging
	 */
	template<int32_t Version = PROTOCOL_VERSION>
	class PlayerActionPacket : public ServerboundPacket {
	public:
		static constexpr int32_t PacketId = Serverbound::Play::PlayerAction;
		static constexpr ConnectionState PacketState = ConnectionState::Play;

		void Decode(NetworkBuffer& buffer) override {
			action = buffer.ReadVarInt();
			const int64_t encoded = buffer.ReadLong();
			// Decode packed position
			blockX = static_cast<int32_t>(encoded >> 38);
			blockY = static_cast<int32_t>((encoded << 52) >> 52);  // Sign-extend 12-bit
			blockZ = static_cast<int32_t>((encoded << 26) >> 38);  // Sign-extend 26-bit
			face = buffer.ReadByte();
			sequence = buffer.ReadVarInt();
		}

		void Handle(Ref<Connection> connection, PacketContext& context) override;

		int32_t action = 0;
		int32_t blockX = 0;
		int32_t blockY = 0;
		int32_t blockZ = 0;
		uint8_t face = 0;
		int32_t sequence = 0;
	};

}
