#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Network/Packet/ServerboundPacket.h"
#include "Axiom/Network/NetworkBuffer.h"

namespace Axiom {

class HandshakePacket : public ServerboundPacket {
public:
    static constexpr int32_t PacketId = 0;
    static constexpr ConnectionState PacketState = ConnectionState::Handshake;

    HandshakePacket() = default;
    HandshakePacket(HandshakePacket&&) = default;
    HandshakePacket& operator=(HandshakePacket&&) = default;

    HandshakePacket(int32_t protocol_version, std::string server_address, uint16_t server_port, int32_t next_state)
        : m_protocol_version(std::move(protocol_version)), m_server_address(std::move(server_address)), m_server_port(std::move(server_port)), m_next_state(std::move(next_state))
    {}

    int32_t GetProtocolVersion() const { return m_protocol_version; }
    void SetProtocolVersion(int32_t value) { m_protocol_version = std::move(value); }
    std::string GetServerAddress() const { return m_server_address; }
    void SetServerAddress(std::string value) { m_server_address = std::move(value); }
    uint16_t GetServerPort() const { return m_server_port; }
    void SetServerPort(uint16_t value) { m_server_port = std::move(value); }
    int32_t GetNextState() const { return m_next_state; }
    void SetNextState(int32_t value) { m_next_state = std::move(value); }

    void Deserialize(NetworkBuffer& buffer) override;
    void Handle(Ref<Connection> connection, PacketContext& context) override;

private:
    int32_t m_protocol_version;
    std::string m_server_address;
    uint16_t m_server_port;
    int32_t m_next_state;
};

} // namespace Axiom