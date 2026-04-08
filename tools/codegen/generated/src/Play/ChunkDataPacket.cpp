#include "ChunkDataPacket.h"
namespace Axiom {

void ChunkDataPacket::Serialize(NetworkBuffer& buffer) const {
    buffer.WriteInt(m_chunk_x);
    buffer.WriteInt(m_chunk_z);
    buffer.WriteNBT(m_heightmaps);
    buffer.WriteByteArray(m_chunk_sections);
    buffer.WriteBlockEntities(m_block_entities);
}

template class ChunkDataPacket<775>;

} // namespace Axiom