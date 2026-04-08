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
        : m_uuid(std::move(uuid)), m_username(std::move(username))
    {}

    UUID GetUuid() const { return m_uuid; }
    void SetUuid(UUID value) { m_uuid = std::move(value); }
    std::string GetUsername() const { return m_username; }
    void SetUsername(std::string value) { m_username = std::move(value); }

    int32_t GetPacketId() const override { return PacketId; }
    void Serialize(NetworkBuffer& buffer) const override;

private:
    UUID m_uuid;
    std::string m_username;
};

} // namespace Axiom