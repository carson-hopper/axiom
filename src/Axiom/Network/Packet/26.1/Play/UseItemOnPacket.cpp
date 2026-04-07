#include "UseItemOnPacket.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"
#include "Axiom/Environment/World/Generator/BlockStates.h"

namespace Axiom {

	template<int32_t Version>
	void UseItemOnPacket<Version>::Handle(const Ref<Connection> connection, PacketContext& context) {
		// Calculate the position where the new block should be placed
		int32_t placeX = blockX;
		int32_t placeY = blockY;
		int32_t placeZ = blockZ;

		// Offset by face direction
		switch (face) {
			case 0: placeY--; break;  // Bottom
			case 1: placeY++; break;  // Top
			case 2: placeZ--; break;  // North
			case 3: placeZ++; break;  // South
			case 4: placeX--; break;  // West
			case 5: placeX++; break;  // East
			default: break;
		}

		AX_CORE_TRACE("Block place at ({}, {}, {}) face={} from {}", placeX, placeY, placeZ,
			face, connection->RemoteAddress());

		// For now, place stone (proper item handling would check held item)
		// The client already shows the block, we just need to confirm and track it
		context.Ticker().SetBlock(placeX, placeY, placeZ, BlockState::Stone);

		// Send block changed ack
		NetworkBuffer ackPayload;
		ackPayload.WriteVarInt(sequence);
		connection->SendRawPacket(Clientbound::Play::BlockChangedAck, ackPayload);
	}

	template class UseItemOnPacket<775>;

}
