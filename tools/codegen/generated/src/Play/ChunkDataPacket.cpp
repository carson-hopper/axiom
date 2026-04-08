#include "ChunkDataPacket.h"
namespace Axiom {

void ChunkDataPacket::Serialize(NetworkBuffer& buffer) const {
    buffer.WriteInt(mChunkX);
    buffer.WriteInt(mChunkZ);
    buffer.WriteNBT(mHeightmaps);
    buffer.WriteByteArray(mChunkSections);
    buffer.WriteBlockEntities(mBlockEntities);
}

template class ChunkDataPacket<775>;

} // namespace Axiom