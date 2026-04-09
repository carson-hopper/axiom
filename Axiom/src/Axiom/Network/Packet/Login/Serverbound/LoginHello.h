#pragma once

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"
#include "Axiom/Config/ServerConfig.h"

namespace Axiom::Login::Serverbound {

class LoginHelloPacket : public Packet<LoginHelloPacket,
	PID_LOGIN_SB_LOGINHELLO> {
public:
    std::optional<std::vector<Ref<IChainablePacket>>>
    Handle(const Ref<Connection> &connection, PacketContext& context, NetworkBuffer&) {
        AX_CORE_INFO("Login request from {} ({})",
            m_PlayerName.Value, connection->RemoteAddress());

        if (context.Config().OnlineMode()) {
            const auto verifyToken = context.GenerateVerifyToken();
            context.StorePendingLogin(connection->Id(),
                PendingLogin{m_PlayerName.Value, verifyToken});

            NetworkBuffer payload;
            payload.WriteString("");
            payload.WriteVarInt(
                static_cast<int32_t>(context.KeyPair().PublicKeyDer().size()));
            payload.WriteBytes(context.KeyPair().PublicKeyDer());
            payload.WriteVarInt(static_cast<int32_t>(verifyToken.size()));
            payload.WriteBytes(verifyToken.data(), verifyToken.size());
            payload.WriteBoolean(true);

            connection->SendRawPacket(
                Clientbound::Login::Hello, payload);
        } else {
            std::string offlineUuid =
                "00000000-0000-3000-8000-"
                + m_PlayerName.Value.substr(0, 12);
            while (offlineUuid.size() < 36) {
                offlineUuid += "0";
            }
            context.CompleteLogin(connection, offlineUuid,
                m_PlayerName.Value);
        }

        return std::nullopt;
    }

    AX_START_FIELDS()
        AX_DECLARE(PlayerName)
    AX_END_FIELDS()

    AX_FIELD(PlayerName, std::string);
};

} // namespace Axiom::Login::Serverbound
