#pragma once

#include "Axiom/Core/UUID.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Environment/Entity/Player.h"

namespace Axiom::Play::Clientbound {

/**
 * Sends player info to clients including skin textures.
 * Action bitmask determines which fields are sent.
 */
class PlayerInfoUpdatePacket : public Packet<PlayerInfoUpdatePacket,
	PID_PLAY_CB_PLAYERINFOUPDATE> {
public:
	PlayerInfoUpdatePacket() = default;

	/** Add a player entry with their properties (including skin). */
	void AddPlayer(const Ref<Player>& player) {
		m_Players.push_back(player);
	}

	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>&, PacketContext&, NetworkBuffer&) { return std::nullopt; }

	void Write(NetworkBuffer& buffer) override {
		// Actions bitmask: 0x01 = Add Player, 0x08 = Update Listed
		uint8_t actions = 0x01 | 0x08;
		buffer.WriteByte(actions);

		buffer.WriteVarInt(static_cast<int32_t>(m_Players.size()));

		for (const auto& player : m_Players) {
			// Write UUID
			const auto& uuid = player->GetUuid();
			buffer.WriteLong(static_cast<int64_t>(uuid.MostSignificantBits()));
			buffer.WriteLong(static_cast<int64_t>(uuid.LeastSignificantBits()));

			// Action 0x01: Add Player
			buffer.WriteString(player->Name());

			// Properties (skin textures)
			const auto& properties = player->Properties();
			buffer.WriteVarInt(static_cast<int32_t>(properties.size()));
			for (const auto& property : properties) {
				buffer.WriteString(property.Name);
				buffer.WriteString(property.Value);
				if (!property.Signature.empty()) {
					buffer.WriteBoolean(true);
					buffer.WriteString(property.Signature);
				} else {
					buffer.WriteBoolean(false);
				}
			}

			// Action 0x08: Update Listed
			buffer.WriteBoolean(true);
		}
	}

	auto Fields() { return std::tuple<>(); }

private:
	std::vector<Ref<Player>> m_Players;
};

} // namespace Axiom::Play::Clientbound
