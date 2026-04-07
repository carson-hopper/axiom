#include "ChunkBatchReceivedPacket.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom {

	template<int32_t Version>
	void ChunkBatchReceivedPacket<Version>::Handle(const Ref<Connection> connection, PacketContext& /*context*/) {
		AX_CORE_TRACE("Chunk batch received from {}: {} chunks/tick",
			connection->RemoteAddress(), chunksPerTick);
	}

	template class ChunkBatchReceivedPacket<775>;

}
