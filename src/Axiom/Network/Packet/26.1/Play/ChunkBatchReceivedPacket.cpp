#include "ChunkBatchReceivedPacket.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom {

PACKET_DECODE_BEGIN(ChunkBatchReceivedPacket)
    READ_FLOAT(m_ChunksPerTick);
PACKET_DECODE_END()

PACKET_HANDLE_BEGIN(ChunkBatchReceivedPacket)
    AX_CORE_TRACE("Chunk batch received from {}: {} chunks/tick",
        connection->RemoteAddress(), m_ChunksPerTick);
PACKET_HANDLE_END()

PACKET_INSTANTIATE(ChunkBatchReceivedPacket, 775)

}
