#pragma once

#include "Axiom/Network/Protocol.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"
#include "Axiom/Network/Packet/Status/Clientbound/StatusResponse.h"
#include "Axiom/Config/ServerConfig.h"

#include <nlohmann/json.hpp>

namespace Axiom::Status::Serverbound {

class StatusRequestPacket : public Packet<StatusRequestPacket,
	PID_STATUS_SB_STATUSREQUEST> {
public:
	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>&, PacketContext& context, NetworkBuffer&) {
		nlohmann::json response;
		response["version"]["name"] = MINECRAFT_VERSION;
		response["version"]["protocol"] = PROTOCOL_VERSION;
		response["players"]["max"] = context.Config().MaxPlayers();
		response["players"]["online"] = 0;
		response["players"]["sample"] = nlohmann::json::array();
		response["description"]["text"] = context.Config().Motd();
		response["enforcesSecureChat"] = false;

		return CreateChainPacketsWithArgs<Clientbound::StatusResponsePacket>(
			std::make_tuple(response.dump()));
	}

	auto Fields() { return std::tuple<>(); }
};

} // namespace Axiom::Status::Serverbound
