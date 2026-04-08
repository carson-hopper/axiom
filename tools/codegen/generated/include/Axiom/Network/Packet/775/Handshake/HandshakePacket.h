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

    HandshakePacket(int32_t protocolVersion, std::string serverAddress, uint16_t serverPort, int32_t nextState)
        : mProtocolVersion(std::move(protocolVersion)), mServerAddress(std::move(serverAddress)), mServerPort(std::move(serverPort)), mNextState(std::move(nextState))
    {}

    int32_t GetProtocolVersion() const { return mProtocolVersion; }
    void SetProtocolVersion(int32_t value) { mProtocolVersion = std::move(value); }
    std::string GetServerAddress() const { return mServerAddress; }
    void SetServerAddress(std::string value) { mServerAddress = std::move(value); }
    uint16_t GetServerPort() const { return mServerPort; }
    void SetServerPort(uint16_t value) { mServerPort = std::move(value); }
    int32_t GetNextState() const { return mNextState; }
    void SetNextState(int32_t value) { mNextState = std::move(value); }

    void Deserialize(NetworkBuffer& buffer) override;
    void Handle(Ref<Connection> connection, PacketContext& context) override;

private:
    int32_t mProtocolVersion;
    std::string mServerAddress;
    uint16_t mServerPort;
    int32_t mNextState;
};

} // namespace Axiom