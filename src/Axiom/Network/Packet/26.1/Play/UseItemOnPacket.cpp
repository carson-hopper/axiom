#include "UseItemOnPacket.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Core/Error.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"
#include "Axiom/Environment/Level/Generator/BlockStates.h"

namespace Axiom {

	// ----- Packet Decoding ------------------------------------------

	PACKET_DECODE_BEGIN(UseItemOnPacket)
		READ_VARINT(m_Hand)
		READ_BLOCK_POS(m_BlockX, m_BlockY, m_BlockZ)
		READ_FLOAT(m_CursorX)
		READ_FLOAT(m_CursorY)
		READ_FLOAT(m_CursorZ)
		READ_BOOL(m_InsideBlock)
		READ_BOOL(m_WorldBorderHit)
		READ_VARINT(m_Sequence)
	PACKET_DECODE_END()

	// ----- Packet Handling ------------------------------------------

	PACKET_HANDLE_BEGIN(UseItemOnPacket)
		// Calculate the position where the block should be placed
		int32_t placeX = m_BlockX;
		int32_t placeY = m_BlockY;
		int32_t placeZ = m_BlockZ;

		switch (m_Face) {
			case 0: placeY--; break; // Bottom
			case 1: placeY++; break; // Top
			case 2: placeZ--; break; // North
			case 3: placeZ++; break; // South
			case 4: placeX--; break; // West
			case 5: placeX++; break; // East
			default:
				AX_CORE_WARN("Invalid block face {} from {}", m_Face, connection->RemoteAddress());
				break;
		}

		// Look up the held item and convert to block state
		const auto player = context.Players().GetPlayer(connection->Id());
		int32_t blockState = 0;

		if (player) {
			const int32_t heldItemId = player->GetHeldItemId();
			blockState = context.ItemBlocks().GetBlockState(heldItemId);
		} else {
			AX_CORE_WARN("UseItemOn from unknown player {}", connection->RemoteAddress());
		}

		// Send acknowledgment regardless of whether block was placed
		NetworkBuffer ackPayload;
		ackPayload.WriteVarInt(m_Sequence);
		connection->SendRawPacket(Clientbound::Play::BlockChangedAck, ackPayload);

		// If not a block item, just ack and return
		if (blockState == 0) {
			return;
		}

		AX_CORE_TRACE("Block place at ({}, {}, {}) state={} from {}",
			placeX, placeY, placeZ, blockState, connection->RemoteAddress());

		// Set the block with error handling
		if (auto result = context.Ticker().SetBlock(placeX, placeY, placeZ, blockState);!result) {
			AX_CORE_WARN("Failed to place block at ({}, {}, {}): {}",
				placeX, placeY, placeZ, result.error().message());
		}
	PACKET_HANDLE_END()

	// ----- Template Instantiation -----------------------------------

	PACKET_INSTANTIATE(UseItemOnPacket, 775)

}