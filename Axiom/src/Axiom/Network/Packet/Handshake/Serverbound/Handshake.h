#pragma once

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom::Handshake::Serverbound {

class HandshakePacket : public Packet<HandshakePacket,
    PID_HANDSHAKE_SB_HANDSHAKE> {
public:
    std::optional<std::vector<Ref<IChainablePacket>>>
    Handle(const Ref<Connection> &connection, PacketContext& /*context*/, NetworkBuffer&) {
        AX_CORE_TRACE("Handshake from {}: protocol={}, address={}:{}, nextState={}",
            connection->RemoteAddress(), m_ProtocolVersion.Value,
            m_ServerAddress.Value, m_ServerPort.Value, m_NextState.Value);

        connection->ProtocolVersion(m_ProtocolVersion.Value);

        if (m_NextState.Value == 1) {
            connection->State(ConnectionState::Status);
        } else if (m_NextState.Value == 2) {
            connection->State(ConnectionState::Login);
        } else {
            connection->Disconnect("Invalid next state");
        }

        return std::nullopt;
    }

    AX_START_FIELDS()
        AX_DECLARE(ProtocolVersion),
        AX_DECLARE(ServerAddress),
        AX_DECLARE(ServerPort),
        AX_DECLARE(NextState)
    AX_END_FIELDS()

    AX_FIELD(ProtocolVersion, int32_t);
    AX_FIELD(ServerAddress, std::string);
    AX_FIELD(ServerPort, uint16_t);
    AX_FIELD(NextState, int32_t);
};

} // namespace Axiom::Handshake::Serverbound
