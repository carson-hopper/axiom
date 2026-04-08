#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Network/Packet/ClientboundPacket.h"
#include "Axiom/Network/NetworkBuffer.h"
#include <vector>

namespace Axiom {

class ChunkDataPacket : public ClientboundPacket {
public:
    static constexpr int32_t PacketId = 37;
    static constexpr ConnectionState PacketState = ConnectionState::Play;

    ChunkDataPacket() = default;
    ChunkDataPacket(ChunkDataPacket&&) = default;
    ChunkDataPacket& operator=(ChunkDataPacket&&) = default;

    ChunkDataPacket(int32_t chunk_x, int32_t chunk_z, nbt::NBT heightmaps, std::vector<uint8_t> chunk_sections, std::vector<BlockEntity> block_entities)
        : m_chunk_x(std::move(chunk_x)), m_chunk_z(std::move(chunk_z)), m_heightmaps(std::move(heightmaps)), m_chunk_sections(std::move(chunk_sections)), m_block_entities(std::move(block_entities))
    {}

    int32_t GetChunkX() const { return m_chunk_x; }
    void SetChunkX(int32_t value) { m_chunk_x = std::move(value); }
    int32_t GetChunkZ() const { return m_chunk_z; }
    void SetChunkZ(int32_t value) { m_chunk_z = std::move(value); }
    nbt::NBT GetHeightmaps() const { return m_heightmaps; }
    void SetHeightmaps(nbt::NBT value) { m_heightmaps = std::move(value); }
    std::vector<uint8_t> GetChunkSections() const { return m_chunk_sections; }
    void SetChunkSections(std::vector<uint8_t> value) { m_chunk_sections = std::move(value); }
    std::vector<BlockEntity> GetBlockEntities() const { return m_block_entities; }
    void SetBlockEntities(std::vector<BlockEntity> value) { m_block_entities = std::move(value); }

    int32_t GetPacketId() const override { return PacketId; }
    void Serialize(NetworkBuffer& buffer) const override;

private:
    int32_t m_chunk_x;
    int32_t m_chunk_z;
    nbt::NBT m_heightmaps;
    std::vector<uint8_t> m_chunk_sections;
    std::vector<BlockEntity> m_block_entities;
};

} // namespace Axiom