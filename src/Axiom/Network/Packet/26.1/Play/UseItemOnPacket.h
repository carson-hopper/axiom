#pragma once

#include "Axiom/Network/Packet/ServerboundPacket.h"
#include "Axiom/Network/Protocol.h"

#include <cstdint>

namespace Axiom {

	/**
	 * Player right-clicks on a block face to place a block or interact.
	 */
	template<int32_t Version = PROTOCOL_VERSION>
	class UseItemOnPacket : public ServerboundPacket {
	public:
		static constexpr int32_t PacketId = Serverbound::Play::UseItemOn;
		static constexpr ConnectionState PacketState = ConnectionState::Play;

		void Decode(NetworkBuffer& buffer) override {
			hand = buffer.ReadVarInt();
			const int64_t encoded = buffer.ReadLong();
			blockX = static_cast<int32_t>(encoded >> 38);
			blockY = static_cast<int32_t>((encoded << 52) >> 52);
			blockZ = static_cast<int32_t>((encoded << 26) >> 38);
			face = buffer.ReadVarInt();
			cursorX = buffer.ReadFloat();
			cursorY = buffer.ReadFloat();
			cursorZ = buffer.ReadFloat();
			insideBlock = buffer.ReadBoolean();
			worldBorderHit = buffer.ReadBoolean();
			sequence = buffer.ReadVarInt();
		}

		void Handle(Ref<Connection> connection, PacketContext& context) override;

		int32_t hand = 0;
		int32_t blockX = 0;
		int32_t blockY = 0;
		int32_t blockZ = 0;
		int32_t face = 0;
		float cursorX = 0;
		float cursorY = 0;
		float cursorZ = 0;
		bool insideBlock = false;
		bool worldBorderHit = false;
		int32_t sequence = 0;
	};

}
