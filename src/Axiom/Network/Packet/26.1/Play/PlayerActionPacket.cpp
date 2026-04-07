#include "PlayerActionPacket.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Core/Error.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"
#include "Axiom/Environment/Level/Generator/BlockStates.h"

namespace Axiom {

	// ----- Packet Decoding ------------------------------------------

	PACKET_DECODE_BEGIN(PlayerActionPacket)
		READ_VARINT(m_Action);
		READ_BLOCK_POS(m_BlockX, m_BlockY, m_BlockZ);
		READ_BYTE(m_Face);
		READ_VARINT(m_Sequence);
	PACKET_DECODE_END()

	// ----- Packet Handling ------------------------------------------

	PACKET_HANDLE_BEGIN(PlayerActionPacket)
		// Action 0 = Started digging (instant break in creative)
		// Action 2 = Finished digging (survival)
		if (m_Action == 0 || m_Action == 2) {
			AX_CORE_TRACE("Block break at ({}, {}, {}) from {}",
				m_BlockX, m_BlockY, m_BlockZ, connection->RemoteAddress());

			// Break the block (set to air) with error handling
			auto result = context.Ticker().SetBlock(m_BlockX, m_BlockY, m_BlockZ, BlockState::Air);
			if (!result) {
				AX_CORE_WARN("Failed to break block at ({}, {}, {}): {}",
					m_BlockX, m_BlockY, m_BlockZ, result.error().message());
			}

			// Send block changed ack
			NetworkBuffer ackPayload;
			ackPayload.WriteVarInt(m_Sequence);
			connection->SendRawPacket(Clientbound::Play::BlockChangedAck, ackPayload);
		}
	PACKET_HANDLE_END()

	// ----- Template Instantiation -----------------------------------

	PACKET_INSTANTIATE(PlayerActionPacket, 775)

}