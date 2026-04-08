#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Network/Packet/ClientboundPacket.h"
#include "Axiom/Network/NetworkBuffer.h"

namespace Axiom {

class KeepAliveResponsePacket : public ClientboundPacket {
public:
    static constexpr int32_t PacketId = 36;
    static constexpr ConnectionState PacketState = ConnectionState::Play;

    KeepAliveResponsePacket() = default;
    KeepAliveResponsePacket(KeepAliveResponsePacket&&) = default;
    KeepAliveResponsePacket& operator=(KeepAliveResponsePacket&&) = default;

    KeepAliveResponsePacket(int64_t id)
        : mId(std::move(id))
    {}

    int64_t GetId() const { return mId; }
    void SetId(int64_t value) { mId = std::move(value); }

    int32_t GetPacketId() const override { return PacketId; }
    void Serialize(NetworkBuffer& buffer) const override;

private:
    int64_t mId;
};

} // namespace Axiom