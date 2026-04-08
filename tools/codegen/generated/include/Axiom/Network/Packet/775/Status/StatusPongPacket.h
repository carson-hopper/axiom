#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Network/Packet/ClientboundPacket.h"
#include "Axiom/Network/NetworkBuffer.h"

namespace Axiom {

class StatusPongPacket : public ClientboundPacket {
public:
    static constexpr int32_t PacketId = 1;
    static constexpr ConnectionState PacketState = ConnectionState::Status;

    StatusPongPacket() = default;
    StatusPongPacket(StatusPongPacket&&) = default;
    StatusPongPacket& operator=(StatusPongPacket&&) = default;

    StatusPongPacket(int64_t payload)
        : m_payload(std::move(payload))
    {}

    int64_t GetPayload() const { return m_payload; }
    void SetPayload(int64_t value) { m_payload = std::move(value); }

    int32_t GetPacketId() const override { return PacketId; }
    void Serialize(NetworkBuffer& buffer) const override;

private:
    int64_t m_payload;
};

} // namespace Axiom