#pragma once

#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"

#include <string>
#include <utility>
#include <vector>

namespace Axiom::Play::Clientbound {

/**
 * Response to a CommandSuggestion request.
 * Carries the completion matches for the client to render.
 */
class CommandSuggestionsPacket : public Packet<CommandSuggestionsPacket,
	PID_PLAY_CB_COMMANDSUGGESTIONS> {
public:
	CommandSuggestionsPacket() = default;

	CommandSuggestionsPacket(int32_t transactionId, int32_t start, int32_t length,
		std::vector<std::string> matches)
		: m_TransactionId(transactionId)
		, m_Start(start)
		, m_Length(length)
		, m_Matches(std::move(matches)) {}

	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>&, PacketContext&, NetworkBuffer&) { return std::nullopt; }

	void Write(NetworkBuffer& buffer) override {
		buffer.WriteVarInt(m_TransactionId);
		buffer.WriteVarInt(m_Start);
		buffer.WriteVarInt(m_Length);
		buffer.WriteVarInt(static_cast<int32_t>(m_Matches.size()));
		for (const auto& match : m_Matches) {
			buffer.WriteString(match);
			buffer.WriteBoolean(false); // No tooltip
		}
	}

	auto Fields() { return std::tuple<>(); }

private:
	int32_t m_TransactionId = 0;
	int32_t m_Start = 0;
	int32_t m_Length = 0;
	std::vector<std::string> m_Matches;
};

} // namespace Axiom::Play::Clientbound
