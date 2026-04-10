#pragma once

#include "Axiom/Command/CommandRegistry.h"
#include "Axiom/Command/CommandSourceStack.h"
#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom::Play::Serverbound {

/**
 * Sent when the player types a slash command (e.g. "/gamemode creative").
 * The command string does NOT include the leading slash.
 */
class ChatCommandPacket : public Packet<ChatCommandPacket, PID_PLAY_SB_CHATCOMMAND> {
public:
	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>& connection, PacketContext& context, NetworkBuffer&) {
		auto player = context.Server().GetPlayer(connection);
		if (!player) {
			return std::nullopt;
		}

		AX_CORE_INFO("{} issued command: /{}", player->Name(), m_Command.Value);

		CommandSourceStack source(player->Name(),
			static_cast<int>(player->GetOpLevel()),
			player->GetPosition(), player);
		context.Commands().Dispatch(source, m_Command.Value);
		return std::nullopt;
	}

	AX_START_FIELDS()
		AX_DECLARE(Command)
	AX_END_FIELDS()

	AX_FIELD(Command, std::string)
};

} // namespace Axiom::Play::Serverbound
