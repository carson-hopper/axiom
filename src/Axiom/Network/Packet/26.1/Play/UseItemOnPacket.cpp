#include "UseItemOnPacket.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"
#include "Axiom/Environment/Level/Generator/BlockStates.h"

namespace Axiom {

	template<int32_t Version>
	void UseItemOnPacket<Version>::Handle(const Ref<Connection> connection, PacketContext& context) {
		int32_t placeX = m_BlockX;
		int32_t placeY = m_BlockY;
		int32_t placeZ = m_BlockZ;

		switch (m_Face) {
			case 0: placeY--; break;
			case 1: placeY++; break;
			case 2: placeZ--; break;
			case 3: placeZ++; break;
			case 4: placeX--; break;
			case 5: placeX++; break;
			default: break;
		}

		// Look up the held item and convert to block state
		auto player = context.Players().GetPlayer(connection.get());
		int32_t blockState = 0;

		if (player) {
			const int32_t heldItemId = player->GetHeldItemId();
			blockState = context.ItemBlocks().GetBlockState(heldItemId);
		}

		if (blockState == 0) {
			// Not a block item — just ack and return
			NetworkBuffer ackPayload;
			ackPayload.WriteVarInt(m_Sequence);
			connection->SendRawPacket(Clientbound::Play::BlockChangedAck, ackPayload);
			return;
		}

		AX_CORE_TRACE("Block place at ({}, {}, {}) state={} from {}",
			placeX, placeY, placeZ, blockState, connection->RemoteAddress());

		context.Ticker().SetBlock(placeX, placeY, placeZ, blockState);

		NetworkBuffer ackPayload;
		ackPayload.WriteVarInt(m_Sequence);
		connection->SendRawPacket(Clientbound::Play::BlockChangedAck, ackPayload);
	}

	template class UseItemOnPacket<775>;

}
