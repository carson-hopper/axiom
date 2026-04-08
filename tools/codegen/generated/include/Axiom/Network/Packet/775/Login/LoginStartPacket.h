#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Network/Packet/ServerboundPacket.h"
#include "Axiom/Network/NetworkBuffer.h"

namespace Axiom {

class LoginStartPacket : public ServerboundPacket {
public:
    static constexpr int32_t PacketId = 0;
    static constexpr ConnectionState PacketState = ConnectionState::Login;

    LoginStartPacket() = default;
    LoginStartPacket(LoginStartPacket&&) = default;
    LoginStartPacket& operator=(LoginStartPacket&&) = default;

    LoginStartPacket(std::string username)
        : m_username(std::move(username))
    {}

    std::string GetUsername() const { return m_username; }
    void SetUsername(std::string value) { m_username = std::move(value); }

    void Deserialize(NetworkBuffer& buffer) override;
    void Handle(Ref<Connection> connection, PacketContext& context) override;

private:
    std::string m_username;
};

} // namespace Axiom