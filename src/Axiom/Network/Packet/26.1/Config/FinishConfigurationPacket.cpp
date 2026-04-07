#include "FinishConfigurationPacket.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Protocol.h"
#include "Axiom/Network/Packet/PacketContext.h"
#include "Axiom/Config/ServerConfig.h"
#include "Axiom/World/ChunkEncoder.h"

namespace Axiom {

	namespace {

		void SendLoginPacket(const Ref<Connection>& connection, PacketContext& context) {
			NetworkBuffer payload;

			payload.WriteInt(1);           // Entity ID
			payload.WriteBoolean(false);   // Is hardcore
			payload.WriteVarInt(1);        // Dimension count
			payload.WriteString("minecraft:overworld");

			payload.WriteVarInt(context.Config().MaxPlayers());
			payload.WriteVarInt(context.Config().ViewDistance());
			payload.WriteVarInt(context.Config().ViewDistance());  // Simulation distance
			payload.WriteBoolean(false);   // Reduced debug info
			payload.WriteBoolean(true);    // Enable respawn screen
			payload.WriteBoolean(false);   // Do limited crafting
			payload.WriteVarInt(1);        // Dimension type (Holder: registry index 0 + 1)
			payload.WriteString("minecraft:overworld");  // Dimension name
			payload.WriteLong(0);          // Hashed seed
			payload.WriteByte(1);          // Game mode (creative)
			payload.WriteByte(-1);         // Previous game mode (none)
			payload.WriteBoolean(false);   // Is debug
			payload.WriteBoolean(true);    // Is flat
			payload.WriteBoolean(false);   // Has death location
			payload.WriteVarInt(0);        // Portal cooldown
			payload.WriteVarInt(0);        // Sea level
			payload.WriteBoolean(false);   // Enforces secure chat

			connection->SendRawPacket(Clientbound::Play::Login, payload);
		}

		void SendSpawnPosition(const Ref<Connection>& connection) {
			NetworkBuffer payload;

			payload.WriteString("minecraft:overworld");  // Dimension name

			// Position as packed long: x(26 bits) | z(26 bits) | y(12 bits)
			int64_t x = 0, y = 0, z = 0;
			const int64_t position = ((x & 0x3FFFFFF) << 38) | ((z & 0x3FFFFFF) << 12) | (y & 0xFFF);
			payload.WriteLong(position);

			payload.WriteFloat(0.0f);  // Yaw
			payload.WriteFloat(0.0f);  // Pitch

			connection->SendRawPacket(Clientbound::Play::SetDefaultSpawnPosition, payload);
		}

		void SendPlayerPosition(const Ref<Connection>& connection) {
			NetworkBuffer payload;

			payload.WriteVarInt(0); // Teleport ID
			payload.WriteVector3(0.5, 0.0, 0.5); // Position
			payload.WriteVector3(0.0, 0.0, 0.0); // Velocity
			payload.WriteVector2(0.0f, 0.0f); // Rotation
			payload.WriteInt(0); // Flags (all absolute)

			connection->SendRawPacket(Clientbound::Play::PlayerPosition, payload);
		}

		void SendChunks(const Ref<Connection>& connection, const int viewDistance) {
			// Set chunk cache center
			{
				NetworkBuffer payload;
				payload.WriteVarInt(0);
				payload.WriteVarInt(0);
				connection->SendRawPacket(Clientbound::Play::SetChunkCacheCenter, payload);
			}

			// Chunk batch start
			{
				const NetworkBuffer payload;
				connection->SendRawPacket(Clientbound::Play::ChunkBatchStart, payload);
			}

			int radius = std::min(viewDistance, 3);
			for (int x = -radius; x <= radius; x++) {
				for (int z = -radius; z <= radius; z++) {
					NetworkBuffer payload;
					ChunkEncoder::EncodeLevelChunkWithLight(payload, x, z);
					connection->SendRawPacket(Clientbound::Play::LevelChunkWithLight, payload);
				}
			}

			// Chunk batch finished
			{
				int chunkCount = (radius * 2 + 1) * (radius * 2 + 1);
				NetworkBuffer payload;
				payload.WriteVarInt(chunkCount);
				connection->SendRawPacket(Clientbound::Play::ChunkBatchFinished, payload);
			}
		}

		void SendGameEvent(const Ref<Connection>& connection, uint8_t eventId, float value) {
			NetworkBuffer payload;
			payload.WriteByte(eventId);
			payload.WriteFloat(value);
			connection->SendRawPacket(Clientbound::Play::GameEvent, payload);
		}

	} // anonymous namespace

	template<int32_t Version>
	void FinishConfigurationPacket<Version>::Handle(const Ref<Connection> connection, PacketContext& context) {
		AX_CORE_INFO("Configuration complete for {}", connection->RemoteAddress());
		connection->State(ConnectionState::Play);

		SendLoginPacket(connection, context);
		SendSpawnPosition(connection);
		SendPlayerPosition(connection);
		SendChunks(connection, context.Config().ViewDistance());

		// Game event 13 = "Start waiting for level chunks"
		SendGameEvent(connection, 13, 0.0f);

		AX_CORE_INFO("Player joined the world from {}", connection->RemoteAddress());
	}

	template class FinishConfigurationPacket<775>;

}
