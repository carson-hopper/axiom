#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Network/Packet/ClientboundPacket.h"
#include "Axiom/Network/NetworkBuffer.h"

namespace Axiom {

class StatusResponsePacket : public ClientboundPacket {
public:
    static constexpr int32_t PacketId = 0;
    static constexpr ConnectionState PacketState = ConnectionState::Status;

    StatusResponsePacket() = default;
    StatusResponsePacket(StatusResponsePacket&&) = default;
    StatusResponsePacket& operator=(StatusResponsePacket&&) = default;

    StatusResponsePacket(std::string json_response)
        : m_json_response(std::move(json_response))
    {}

    std::string GetJsonResponse() const { return m_json_response; }
    void SetJsonResponse(std::string value) { m_json_response = std::move(value); }

    int32_t GetPacketId() const override { return PacketId; }
    void Serialize(NetworkBuffer& buffer) const override;

private:
    std::string m_json_response;
};

} // namespace Axiom