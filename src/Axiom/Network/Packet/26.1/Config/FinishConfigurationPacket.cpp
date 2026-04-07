#include "FinishConfigurationPacket.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Protocol.h"
#include "Axiom/Network/Packet/PacketContext.h"
#include "Axiom/Config/ServerConfig.h"
#include "Axiom/World/ChunkEncoder.h"

namespace Axiom {

	namespace {

		void SendLoginPacket(Ref<Connection>& connection, PacketContext& context) {
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

		void SendSpawnPosition(Ref<Connection>& connection) {
			NetworkBuffer payload;

			payload.WriteString("minecraft:overworld");  // Dimension name

			// Position as packed long: x(26 bits) | z(26 bits) | y(12 bits)
			int64_t x = 0, y = 0, z = 0;
			int64_t position = ((x & 0x3FFFFFF) << 38) | ((z & 0x3FFFFFF) << 12) | (y & 0xFFF);
			payload.WriteLong(position);

			payload.WriteFloat(0.0f);  // Yaw
			payload.WriteFloat(0.0f);  // Pitch

			connection->SendRawPacket(Clientbound::Play::SetDefaultSpawnPosition, payload);
		}

		void SendPlayerPosition(Ref<Connection>& connection) {
			NetworkBuffer payload;

			payload.WriteVarInt(0);    // Teleport ID
			payload.WriteDouble(0.5);  // X
			payload.WriteDouble(0.0);  // Y
			payload.WriteDouble(0.5);  // Z
			payload.WriteDouble(0.0);  // Velocity X
			payload.WriteDouble(0.0);  // Velocity Y
			payload.WriteDouble(0.0);  // Velocity Z
			payload.WriteFloat(0.0f);  // Yaw
			payload.WriteFloat(0.0f);  // Pitch
			payload.WriteInt(0);       // Flags (all absolute)

			connection->SendRawPacket(Clientbound::Play::PlayerPosition, payload);
		}

		void SendChunks(Ref<Connection>& connection, int viewDistance) {
			// Set chunk cache center
			{
				NetworkBuffer payload;
				payload.WriteVarInt(0);
				payload.WriteVarInt(0);
				connection->SendRawPacket(Clientbound::Play::SetChunkCacheCenter, payload);
			}

			// Chunk batch start
			{
				NetworkBuffer payload;
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
				NetworkBuffer payload;
				connection->SendRawPacket(Clientbound::Play::ChunkBatchFinished, payload);
			}
		}

		void SendGameEvent(Ref<Connection>& connection, uint8_t eventId, float value) {
			NetworkBuffer payload;
			payload.WriteByte(eventId);
			payload.WriteFloat(value);
			connection->SendRawPacket(Clientbound::Play::GameEvent, payload);
		}

	} // anonymous namespace

	template<int32_t Version>
	void FinishConfigurationPacket<Version>::Handle(Ref<Connection> connection, PacketContext& context) {
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
