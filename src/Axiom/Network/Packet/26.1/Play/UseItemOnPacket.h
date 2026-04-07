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
			m_Hand = buffer.ReadVarInt();
			const int64_t encoded = buffer.ReadLong();
			m_BlockX = static_cast<int32_t>(encoded >> 38);
			m_BlockY = static_cast<int32_t>((encoded << 52) >> 52);
			m_BlockZ = static_cast<int32_t>((encoded << 26) >> 38);
			m_Face = buffer.ReadVarInt();
			m_CursorX = buffer.ReadFloat();
			m_CursorY = buffer.ReadFloat();
			m_CursorZ = buffer.ReadFloat();
			m_InsideBlock = buffer.ReadBoolean();
			m_WorldBorderHit = buffer.ReadBoolean();
			m_Sequence = buffer.ReadVarInt();
		}

		void Handle(Ref<Connection> connection, PacketContext& context) override;

	private:
		int32_t m_Hand = 0;
		int32_t m_BlockX = 0;
		int32_t m_BlockY = 0;
		int32_t m_BlockZ = 0;
		int32_t m_Face = 0;
		float m_CursorX = 0;
		float m_CursorY = 0;
		float m_CursorZ = 0;
		bool m_InsideBlock = false;
		bool m_WorldBorderHit = false;
		int32_t m_Sequence = 0;
	};

}
