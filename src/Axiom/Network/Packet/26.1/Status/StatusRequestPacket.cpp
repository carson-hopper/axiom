#include "StatusRequestPacket.h"

#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"
#include "Axiom/Config/ServerConfig.h"

#include <nlohmann/json.hpp>

namespace Axiom {

	PACKET_HANDLE_BEGIN(StatusRequestPacket)
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
	PACKET_HANDLE_END()

	PACKET_INSTANTIATE(StatusRequestPacket, 775)

}
