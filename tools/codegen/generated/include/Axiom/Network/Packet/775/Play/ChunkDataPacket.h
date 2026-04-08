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

    ChunkDataPacket(int32_t chunkX, int32_t chunkZ, nbt::NBT heightmaps, std::vector<uint8_t> chunkSections, std::vector<BlockEntity> blockEntities)
        : mChunkX(std::move(chunkX)), mChunkZ(std::move(chunkZ)), mHeightmaps(std::move(heightmaps)), mChunkSections(std::move(chunkSections)), mBlockEntities(std::move(blockEntities))
    {}

    int32_t GetChunkX() const { return mChunkX; }
    void SetChunkX(int32_t value) { mChunkX = std::move(value); }
    int32_t GetChunkZ() const { return mChunkZ; }
    void SetChunkZ(int32_t value) { mChunkZ = std::move(value); }
    nbt::NBT GetHeightmaps() const { return mHeightmaps; }
    void SetHeightmaps(nbt::NBT value) { mHeightmaps = std::move(value); }
    std::vector<uint8_t> GetChunkSections() const { return mChunkSections; }
    void SetChunkSections(std::vector<uint8_t> value) { mChunkSections = std::move(value); }
    std::vector<BlockEntity> GetBlockEntities() const { return mBlockEntities; }
    void SetBlockEntities(std::vector<BlockEntity> value) { mBlockEntities = std::move(value); }

    int32_t GetPacketId() const override { return PacketId; }
    void Serialize(NetworkBuffer& buffer) const override;

private:
    int32_t mChunkX;
    int32_t mChunkZ;
    nbt::NBT mHeightmaps;
    std::vector<uint8_t> mChunkSections;
    std::vector<BlockEntity> mBlockEntities;
};

} // namespace Axiom