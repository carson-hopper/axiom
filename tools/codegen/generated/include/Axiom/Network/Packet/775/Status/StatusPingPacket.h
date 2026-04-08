#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Network/Packet/ServerboundPacket.h"
#include "Axiom/Network/NetworkBuffer.h"

namespace Axiom {

class StatusPingPacket : public ServerboundPacket {
public:
    static constexpr int32_t PacketId = 1;
    static constexpr ConnectionState PacketState = ConnectionState::Status;

    StatusPingPacket() = default;
    StatusPingPacket(StatusPingPacket&&) = default;
    StatusPingPacket& operator=(StatusPingPacket&&) = default;

    StatusPingPacket(int64_t payload)
        : m_payload(std::move(payload))
    {}

    int64_t GetPayload() const { return m_payload; }
    void SetPayload(int64_t value) { m_payload = std::move(value); }

    void Deserialize(NetworkBuffer& buffer) override;
    void Handle(Ref<Connection> connection, PacketContext& context) override;

private:
    int64_t m_payload;
};

} // namespace Axiom