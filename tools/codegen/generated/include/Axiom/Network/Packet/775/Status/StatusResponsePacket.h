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

    StatusResponsePacket(std::string jsonResponse)
        : mJsonResponse(std::move(jsonResponse))
    {}

    std::string GetJsonResponse() const { return mJsonResponse; }
    void SetJsonResponse(std::string value) { mJsonResponse = std::move(value); }

    int32_t GetPacketId() const override { return PacketId; }
    void Serialize(NetworkBuffer& buffer) const override;

private:
    std::string mJsonResponse;
};

} // namespace Axiom