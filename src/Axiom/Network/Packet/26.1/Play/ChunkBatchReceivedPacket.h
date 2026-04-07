#pragma once

/**
 * @file ChunkBatchReceivedPacket.h
 *
 * Client reports chunk batch processing rate.
 */

#include "Axiom/Network/Packet/PacketMacros.h"

namespace Axiom {

PACKET_DECL_BEGIN(ChunkBatchReceivedPacket, Play, Serverbound::Play::ChunkBatchReceived)
    PACKET_FIELD_FLOAT(ChunksPerTick)
PACKET_DECL_END()

}
