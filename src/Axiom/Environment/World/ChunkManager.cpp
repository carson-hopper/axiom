#include "ChunkManager.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Environment/World/ChunkEncoder.h"

namespace Axiom {

	void ChunkManager::SendInitialChunks(const Ref<Connection> &connection, const double playerX, const double playerZ) {
		const int32_t chunkX = BlockToChunk(playerX);
		const int32_t chunkZ = BlockToChunk(playerZ);

		// Set chunk cache center
		{
			NetworkBuffer payload;
			payload.WriteVarInt(chunkX);
			payload.WriteVarInt(chunkZ);
			connection->SendRawPacket(Clientbound::Play::SetChunkCacheCenter, payload);
		}

		std::lock_guard<std::mutex> lock(m_Mutex);
		auto& state = m_PlayerStates[connection.get()];
		state.lastChunkX = chunkX;
		state.lastChunkZ = chunkZ;

		// Chunk batch start
		{
			const NetworkBuffer payload;
			connection->SendRawPacket(Clientbound::Play::ChunkBatchStart, payload);
		}

		SendChunksInRadius(connection, chunkX, chunkZ, state);

		// Chunk batch finished
		{
			NetworkBuffer payload;
			payload.WriteVarInt(static_cast<int32_t>(state.loadedChunks.size()));
			connection->SendRawPacket(Clientbound::Play::ChunkBatchFinished, payload);
		}
	}

	void ChunkManager::OnPlayerMove(const Ref<Connection> &connection, double playerX, double playerZ) {
		const int32_t chunkX = BlockToChunk(playerX);
		const int32_t chunkZ = BlockToChunk(playerZ);

		std::lock_guard<std::mutex> lock(m_Mutex);
		const auto iterator = m_PlayerStates.find(connection.get());
		if (iterator == m_PlayerStates.end()) {
			return;
		}

		auto& state = iterator->second;

		// Only update if player crossed a chunk border
		if (chunkX == state.lastChunkX && chunkZ == state.lastChunkZ) {
			return;
		}

		state.lastChunkX = chunkX;
		state.lastChunkZ = chunkZ;

		// Update chunk cache center
		{
			NetworkBuffer payload;
			payload.WriteVarInt(chunkX);
			payload.WriteVarInt(chunkZ);
			connection->SendRawPacket(Clientbound::Play::SetChunkCacheCenter, payload);
		}

		// Unload chunks outside view distance
		UnloadDistantChunks(connection, chunkX, chunkZ, state);

		// Send new chunks in view distance
		{
			const NetworkBuffer payload;
			connection->SendRawPacket(Clientbound::Play::ChunkBatchStart, payload);
		}

		const int chunksBefore = static_cast<int>(state.loadedChunks.size());
		SendChunksInRadius(connection, chunkX, chunkZ, state);

		{
			const int newChunks = static_cast<int>(state.loadedChunks.size()) - chunksBefore;
			NetworkBuffer payload;
			payload.WriteVarInt(std::max(newChunks, 0));
			connection->SendRawPacket(Clientbound::Play::ChunkBatchFinished, payload);
		}
	}

	void ChunkManager::RemovePlayer(Connection* connection) {
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_PlayerStates.erase(connection);
	}

	void ChunkManager::SendChunksInRadius(const Ref<Connection> &connection, const int32_t centerX, const int32_t centerZ,
		PlayerChunkState& state) {

		// Send chunks in a spiral from center outward for better loading UX
		for (int radius = 0; radius <= m_ViewDistance; radius++) {
			for (int x = -radius; x <= radius; x++) {
				for (int z = -radius; z <= radius; z++) {
					// Only process the ring at this radius
					if (std::abs(x) != radius && std::abs(z) != radius) {
						continue;
					}

					ChunkPosition position{centerX + x, centerZ + z};
					if (state.loadedChunks.contains(position)) {
						continue;
					}

					SendChunk(connection, position.x, position.z);
					state.loadedChunks.insert(position);
				}
			}
		}
	}

	void ChunkManager::UnloadDistantChunks(const Ref<Connection> &connection, const int32_t centerX, const int32_t centerZ,
		PlayerChunkState& state) {

		std::vector<ChunkPosition> toUnload;

		for (const auto& position : state.loadedChunks) {
			int distanceX = std::abs(position.x - centerX);
			int distanceZ = std::abs(position.z - centerZ);
			if (distanceX > m_ViewDistance + 1 || distanceZ > m_ViewDistance + 1) {
				toUnload.push_back(position);
			}
		}

		for (const auto& position : toUnload) {
			UnloadChunk(connection, position.x, position.z);
			state.loadedChunks.erase(position);
		}
	}

	void ChunkManager::SendChunk(const Ref<Connection> &connection, const int32_t chunkX, const int32_t chunkZ)const {
		const auto chunkData = m_Generator->Generate(chunkX, chunkZ);

		NetworkBuffer payload;
		payload.WriteInt(chunkX);
		payload.WriteInt(chunkZ);

		ChunkEncoder::EncodeHeightmaps(payload, chunkData.heightmapValue);

		payload.WriteVarInt(static_cast<int32_t>(chunkData.sectionData.size()));
		payload.WriteBytes(chunkData.sectionData);

		payload.WriteVarInt(0); // Block entities

		ChunkEncoder::EncodeLightData(payload);

		connection->SendRawPacket(Clientbound::Play::LevelChunkWithLight, payload);
	}

	void ChunkManager::UnloadChunk(const Ref<Connection> &connection, const int32_t chunkX, const int32_t chunkZ) {
		NetworkBuffer payload;
		payload.WriteInt(chunkX);
		payload.WriteInt(chunkZ);
		connection->SendRawPacket(Clientbound::Play::ForgetLevelChunk, payload);
	}

}
