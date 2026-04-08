#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Network/Packet/ClientboundPacket.h"
#include "Axiom/Network/NetworkBuffer.h"
#include "Axiom/Core/UUID.h"

namespace Axiom {

class LoginSuccessPacket : public ClientboundPacket {
public:
    static constexpr int32_t PacketId = 2;
    static constexpr ConnectionState PacketState = ConnectionState::Login;

    LoginSuccessPacket() = default;
    LoginSuccessPacket(LoginSuccessPacket&&) = default;
    LoginSuccessPacket& operator=(LoginSuccessPacket&&) = default;

    LoginSuccessPacket(UUID uuid, std::string username)
        : mUuid(std::move(uuid)), mUsername(std::move(username))
    {}

    UUID GetUuid() const { return mUuid; }
    void SetUuid(UUID value) { mUuid = std::move(value); }
    std::string GetUsername() const { return mUsername; }
    void SetUsername(std::string value) { mUsername = std::move(value); }

    int32_t GetPacketId() const override { return PacketId; }
    void Serialize(NetworkBuffer& buffer) const override;

private:
    UUID mUuid;
    std::string mUsername;
};

} // namespace Axiom