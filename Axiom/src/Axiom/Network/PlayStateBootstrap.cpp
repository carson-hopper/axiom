#include "PlayStateBootstrap.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Protocol.h"
#include "Axiom/Network/NetworkBuffer.h"
#include "Axiom/Network/Packet/PacketContext.h"
#include "Axiom/Config/ServerConfig.h"
#include "Axiom/Environment/Entity/Player.h"

namespace Axiom {

	void PlayStateBootstrap::Bootstrap(
		const Ref<Connection>& connection,
		PacketContext& context) {

		int32_t entityId = context.Players().NextEntityId();
		const double spawnY = context.ChunkManagement().Generator().SpawnY();

		/**
		 * Create the player entity in PlayerManager.
		 * Uses the connection's pending login info if
		 * available, otherwise falls back to
		 * defaults for offline mode.
		 */
		if (const auto existingPlayer = context.Players().GetPlayer(connection->Id());!existingPlayer) {
			context.Players().AddPlayer(
				entityId, connection, "Player", "");
		} else {
			entityId = existingPlayer->GetEntityId();
		}

		SendLoginPacket(connection, context);
		SendPlayerAbilities(connection);
		SendSpawnPosition(connection, spawnY);
		SendPlayerPosition(connection, spawnY);
		SendInitialChunks(connection, context, spawnY);

		/**
		 * Game event 13 signals the client
		 * to start waiting for level
		 * chunks before rendering.
		 */
		SendGameEvent(connection, 13, 0.0f);

		AX_CORE_INFO("Player joined the world from {}", connection->RemoteAddress());
	}

	void PlayStateBootstrap::SendLoginPacket(
		const Ref<Connection>& connection,
		PacketContext& context) {

		auto player = context.Players().GetPlayer(connection->Id());
		int32_t entityId = player ? player->GetEntityId() : 1;

		NetworkBuffer payload;

		payload.WriteInt(entityId);
		payload.WriteBoolean(false);              // Is hardcore
		payload.WriteVarInt(1);                   // Dimension count
		payload.WriteString("minecraft:overworld");

		payload.WriteVarInt(context.Config().MaxPlayers());
		payload.WriteVarInt(context.Config().ViewDistance());
		payload.WriteVarInt(context.Config().SimulationDistance());
		payload.WriteBoolean(false);              // Reduced debug info
		payload.WriteBoolean(true);               // Enable respawn screen
		payload.WriteBoolean(false);              // Do limited crafting
		payload.WriteVarInt(1);                   // Dimension type (registry index + 1)
		payload.WriteString("minecraft:overworld");
		payload.WriteLong(0);                     // Hashed seed
		payload.WriteByte(static_cast<uint8_t>(GameMode::Creative));
		payload.WriteByte(-1);                    // Previous game mode (none)
		payload.WriteBoolean(false);              // Is debug
		payload.WriteBoolean(false);              // Is flat
		payload.WriteBoolean(false);              // Has death location
		payload.WriteVarInt(0);                   // Portal cooldown
		payload.WriteVarInt(63);                  // Sea level
		payload.WriteBoolean(false);              // Enforces secure chat

		connection->SendRawPacket(Clientbound::Play::Login, payload);
	}

	void PlayStateBootstrap::SendSpawnPosition(
		const Ref<Connection>& connection,
		double spawnY) {

		NetworkBuffer payload;

		payload.WriteString("minecraft:overworld");
		payload.WriteBlockPosition(0, static_cast<int32_t>(spawnY), 0);
		payload.WriteVector2(0.0f, 0.0f);

		connection->SendRawPacket(Clientbound::Play::SetDefaultSpawnPosition, payload);
	}

	void PlayStateBootstrap::SendPlayerPosition(
		const Ref<Connection>& connection,
		double spawnY) {

		NetworkBuffer payload;

		payload.WriteVarInt(0);                   // Teleport ID
		payload.WriteVector3(0.5, spawnY, 0.5);   // Position
		payload.WriteVector3(0.0, 0.0, 0.05);     // Velocity
		payload.WriteVector2(0.0f, 0.0f);          // Rotation
		payload.WriteInt(0);                       // Flags (all absolute)

		connection->SendRawPacket(Clientbound::Play::PlayerPosition, payload);
	}

	void PlayStateBootstrap::SendInitialChunks(
		const Ref<Connection>& connection,
		PacketContext& context,
		[[maybe_unused]] double spawnY) {

		context.ChunkManagement().SendInitialChunks(connection, 0.5, 0.5);
	}

	void PlayStateBootstrap::SendPlayerAbilities(
		const Ref<Connection>& connection) {

		NetworkBuffer payload;

		/**
		 * Ability flags:
		 * 0x01 = invulnerable
		 * 0x02 = flying
		 * 0x04 = allow flying
		 * 0x08 = instant break (creative)
		 */
		constexpr uint8_t flags = 0x01 | 0x04 | 0x08; // Creative: invulnerable, allow flying, instant break
		payload.WriteByte(flags);
		payload.WriteFloat(0.05f);  // fly speed
		payload.WriteFloat(0.1f);   // walk speed (FOV modifier)

		connection->SendRawPacket(Clientbound::Play::PlayerAbilities, payload);
	}

	void PlayStateBootstrap::SendGameEvent(
		const Ref<Connection>& connection,
		uint8_t eventId,
		float value) {

		NetworkBuffer payload;
		payload.WriteByte(eventId);
		payload.WriteFloat(value);

		connection->SendRawPacket(Clientbound::Play::GameEvent, payload);
	}

}
