#pragma once

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom::Play::Serverbound {

class ChunkBatchReceivedPacket : public Packet<ChunkBatchReceivedPacket,
    PID_PLAY_SB_CHUNKBATCHRECEIVED> {
public:
    std::optional<std::vector<Ref<IChainablePacket>>>
    Handle(const Ref<Connection> &connection, PacketContext& /*context*/, NetworkBuffer&) {
        AX_CORE_TRACE("Chunk batch received from {}: {} chunks/tick",
            connection->RemoteAddress(), m_ChunksPerTick.Value);

        return std::nullopt;
    }

    AX_START_FIELDS()
        AX_DECLARE(ChunksPerTick)
    AX_END_FIELDS()

    AX_FIELD(ChunksPerTick, float);
};

} // namespace Axiom::Play::Serverbound
