#include "PlayerActionPacket.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"
#include "Axiom/Environment/World/Generator/BlockStates.h"

namespace Axiom {

	template<int32_t Version>
	void PlayerActionPacket<Version>::Handle(const Ref<Connection> connection, PacketContext& context) {
		// Action 0 = Started digging (instant break in creative)
		// Action 2 = Finished digging (survival)
		if (action == 0 || action == 2) {
			AX_CORE_TRACE("Block break at ({}, {}, {}) from {}", blockX, blockY, blockZ,
				connection->RemoteAddress());

			// Break the block (set to air)
			context.Ticker().SetBlock(blockX, blockY, blockZ, BlockState::Air);

			// Send block changed ack
			NetworkBuffer ackPayload;
			ackPayload.WriteVarInt(sequence);
			connection->SendRawPacket(Clientbound::Play::BlockChangedAck, ackPayload);
		}
	}

	template class PlayerActionPacket<775>;

}
