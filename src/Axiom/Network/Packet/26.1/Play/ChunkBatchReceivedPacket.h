#pragma once

/**
 * @file ChunkBatchReceivedPacket.h
 *
 * Client reports chunk batch processing rate.
 */

#include "Axiom/Network/Packet/PacketVersioned.h"

namespace Axiom {

PACKET_VERSIONED_SB(ChunkBatchReceivedPacket, 775, Play, Serverbound::Play::ChunkBatchReceived)
	FIELD(float, ChunksPerTick, 0.0f)
PACKET_VERSIONED_END(ChunkBatchReceivedPacket, 775)

}
