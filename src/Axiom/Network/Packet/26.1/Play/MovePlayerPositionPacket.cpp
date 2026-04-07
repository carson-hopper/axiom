#include "MovePlayerPositionPacket.h"

#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom {

	template<int32_t Version>
	void MovePlayerPositionPacket<Version>::Handle(const Ref<Connection> connection, PacketContext& context) {
		context.ChunkManagement().OnPlayerMove(connection, x, z);
	}

	template class MovePlayerPositionPacket<775>;

	template<int32_t Version>
	void MovePlayerPositionRotationPacket<Version>::Handle(const Ref<Connection> connection, PacketContext& context) {
		context.ChunkManagement().OnPlayerMove(connection, x, z);
	}

	template class MovePlayerPositionRotationPacket<775>;

	template<int32_t Version>
	void MovePlayerRotationPacket<Version>::Handle(const Ref<Connection> /*connection*/, PacketContext& /*context*/) {
		// No position change — nothing to do for chunks
	}

	template class MovePlayerRotationPacket<775>;

	template<int32_t Version>
	void MovePlayerStatusOnlyPacket<Version>::Handle(const Ref<Connection> /*connection*/, PacketContext& /*context*/) {
		// No position change
	}

	template class MovePlayerStatusOnlyPacket<775>;

}
