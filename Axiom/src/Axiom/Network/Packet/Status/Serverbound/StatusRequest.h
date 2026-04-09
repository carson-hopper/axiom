#pragma once

#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Protocol.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"
#include "Axiom/Config/ServerConfig.h"

#include <nlohmann/json.hpp>

namespace Axiom::Status::Serverbound {

class StatusRequestPacket : public Packet<StatusRequestPacket,
    PID_STATUS_SB_STATUSREQUEST> {
public:
    static std::optional<std::vector<Ref<IChainablePacket>>>
    Handle(const Ref<Connection> &connection, PacketContext& context) {
        nlohmann::json response;
        response["version"]["name"] = MINECRAFT_VERSION;
        response["version"]["protocol"] = PROTOCOL_VERSION;
        response["players"]["max"] = context.Config().MaxPlayers();
        response["players"]["online"] = 0;
        response["players"]["sample"] = nlohmann::json::array();
        response["description"]["text"] = context.Config().Motd();
        response["enforcesSecureChat"] = false;

        NetworkBuffer payload;
        payload.WriteString(response.dump());
        connection->SendRawPacket(Clientbound::Status::StatusResponse, payload);

        return std::nullopt;
    }

	AX_START_FIELDS()

	AX_END_FIELDS()
};

} // namespace Axiom::Status::Serverbound
