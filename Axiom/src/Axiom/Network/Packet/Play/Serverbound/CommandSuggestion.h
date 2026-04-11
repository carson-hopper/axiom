#pragma once

#include "Axiom/Command/CommandRegistry.h"
#include "Axiom/Command/CommandSourceStack.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"
#include "Axiom/Network/Packet/Play/Clientbound/CommandSuggestions.h"

namespace Axiom::Play::Serverbound {

/**
 * Sent by the client while the player is typing a command to
 * request tab-completion suggestions. The server responds with
 * a matching CommandSuggestions packet carrying the same
 * transaction ID.
 */
class CommandSuggestionPacket : public Packet<CommandSuggestionPacket,
	PID_PLAY_SB_COMMANDSUGGESTION> {
public:
	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>& connection, PacketContext& context, NetworkBuffer&) {
		auto player = context.Server().GetPlayer(connection);
		if (!player) {
			return std::nullopt;
		}

		// The typed string includes the leading '/'. Strip it for TabComplete.
		std::string typed = m_Command.Value;
		if (!typed.empty() && typed.front() == '/') {
			typed.erase(0, 1);
		}

		CommandSourceStack source(player->Name(),
			static_cast<int>(player->GetOpLevel()),
			player->GetPosition(), player);
		auto matches = context.Commands().TabComplete(source, typed);

		// The client expects "start" and "length" to identify the substring
		// of its input being replaced. For the simplest case we replace the
		// last whitespace-delimited token.
		const size_t lastSpace = typed.find_last_of(' ');
		const int32_t tokenStart = lastSpace == std::string::npos
			? 1 // after the leading '/'
			: static_cast<int32_t>(lastSpace + 2); // after space, after '/'
		const int32_t tokenLength = static_cast<int32_t>(typed.size()) - (tokenStart - 1);

		auto response = Ref<Clientbound::CommandSuggestionsPacket>::Create(
			m_TransactionId.Value,
			tokenStart,
			tokenLength < 0 ? 0 : tokenLength,
			std::move(matches));

		std::vector<Ref<IChainablePacket>> chain;
		chain.push_back(response);
		return chain;
	}

	AX_START_FIELDS()
		AX_DECLARE(TransactionId),
		AX_DECLARE(Command)
	AX_END_FIELDS()

	AX_FIELD(TransactionId, int32_t)
	AX_FIELD(Command, std::string)
};

} // namespace Axiom::Play::Serverbound
