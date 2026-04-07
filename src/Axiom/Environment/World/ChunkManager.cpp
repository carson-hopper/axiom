#include "ChunkManager.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Environment/World/ChunkEncoder.h"

#include <algorithm>

namespace Axiom {

	ChunkManager::ChunkManager(Ref<ChunkGenerator> generator, int viewDistance)
		: m_Generator(std::move(generator))
		, m_ViewDistance(viewDistance) {

		unsigned int threadCount = std::max(2u, std::thread::hardware_concurrency() / 2);
		for (unsigned int i = 0; i < threadCount; i++) {
			m_Workers.emplace_back(&ChunkManager::WorkerLoop, this);
		}
	}

	ChunkManager::~ChunkManager() {
		m_Stopping = true;
		m_QueueCondition.notify_all();
		for (auto& worker : m_Workers) {
			if (worker.joinable()) {
				worker.join();
			}
		}
	}

	void ChunkManager::SendInitialChunks(Ref<Connection> connection, double playerX, double playerZ) {
		int32_t chunkX = BlockToChunk(playerX);
		int32_t chunkZ = BlockToChunk(playerZ);

		{
			NetworkBuffer payload;
			payload.WriteVarInt(chunkX);
			payload.WriteVarInt(chunkZ);
			connection->SendRawPacket(Clientbound::Play::SetChunkCacheCenter, payload);
		}

		{
			std::lock_guard<std::mutex> lock(m_StateMutex);
			auto& state = m_PlayerStates[connection.get()];
			state.lastChunkX = chunkX;
			state.lastChunkZ = chunkZ;
		}

		{
			NetworkBuffer payload;
			connection->SendRawPacket(Clientbound::Play::ChunkBatchStart, payload);
		}

		QueueChunksInRadius(connection, chunkX, chunkZ);
	}

	void ChunkManager::OnPlayerMove(Ref<Connection> connection, double playerX, double playerZ) {
		int32_t chunkX = BlockToChunk(playerX);
		int32_t chunkZ = BlockToChunk(playerZ);

		bool needsUpdate = false;

		{
			std::lock_guard<std::mutex> lock(m_StateMutex);
			auto iterator = m_PlayerStates.find(connection.get());
			if (iterator == m_PlayerStates.end()) return;

			auto& state = iterator->second;
			if (chunkX == state.lastChunkX && chunkZ == state.lastChunkZ) return;

			state.lastChunkX = chunkX;
			state.lastChunkZ = chunkZ;
			needsUpdate = true;

			UnloadDistantChunks(connection, chunkX, chunkZ, state);
		}

		if (!needsUpdate) return;

		{
			NetworkBuffer payload;
			payload.WriteVarInt(chunkX);
			payload.WriteVarInt(chunkZ);
			connection->SendRawPacket(Clientbound::Play::SetChunkCacheCenter, payload);
		}

		{
			NetworkBuffer payload;
			connection->SendRawPacket(Clientbound::Play::ChunkBatchStart, payload);
		}

		QueueChunksInRadius(connection, chunkX, chunkZ);
	}

	void ChunkManager::RemovePlayer(Connection* connection) {
		std::lock_guard<std::mutex> lock(m_StateMutex);
		m_PlayerStates.erase(connection);
	}

	void ChunkManager::QueueChunksInRadius(Ref<Connection> connection, int32_t centerX, int32_t centerZ) {
		std::vector<ChunkPosition> toGenerate;

		{
			std::lock_guard<std::mutex> lock(m_StateMutex);
			auto stateIterator = m_PlayerStates.find(connection.get());
			if (stateIterator == m_PlayerStates.end()) return;
			auto& state = stateIterator->second;

			for (int radius = 0; radius <= m_ViewDistance; radius++) {
				for (int x = -radius; x <= radius; x++) {
					for (int z = -radius; z <= radius; z++) {
						if (std::abs(x) != radius && std::abs(z) != radius) continue;

						ChunkPosition position{centerX + x, centerZ + z};
						if (state.loadedChunks.contains(position)) continue;

						state.loadedChunks.insert(position);
						toGenerate.push_back(position);
					}
				}
			}
		}

		if (toGenerate.empty()) {
			NetworkBuffer payload;
			payload.WriteVarInt(0);
			connection->SendRawPacket(Clientbound::Play::ChunkBatchFinished, payload);
			return;
		}

		auto remaining = std::make_shared<std::atomic<int>>(static_cast<int>(toGenerate.size()));
		int totalChunks = static_cast<int>(toGenerate.size());

		for (const auto& position : toGenerate) {
			auto connectionRef = connection;
			int32_t chunkX = position.x;
			int32_t chunkZ = position.z;

			SubmitTask([this, connectionRef, chunkX, chunkZ, remaining, totalChunks]() {
				if (!connectionRef->IsConnected()) return;

				SendChunk(connectionRef, chunkX, chunkZ);

				if (remaining->fetch_sub(1) == 1) {
					NetworkBuffer payload;
					payload.WriteVarInt(totalChunks);
					connectionRef->SendRawPacket(Clientbound::Play::ChunkBatchFinished, payload);
				}
			});
		}
	}

	void ChunkManager::UnloadDistantChunks(Ref<Connection> connection, int32_t centerX, int32_t centerZ,
		PlayerChunkState& state) {

		std::vector<ChunkPosition> toUnload;

		for (const auto& position : state.loadedChunks) {
			if (std::abs(position.x - centerX) > m_ViewDistance + 1 ||
				std::abs(position.z - centerZ) > m_ViewDistance + 1) {
				toUnload.push_back(position);
			}
		}

		for (const auto& position : toUnload) {
			UnloadChunk(connection, position.x, position.z);
			state.loadedChunks.erase(position);
		}
	}

	void ChunkManager::SendChunk(const Ref<Connection>& connection, int32_t chunkX, int32_t chunkZ) const {
		auto chunkData = m_Generator->Generate(chunkX, chunkZ);

		NetworkBuffer payload;
		payload.WriteInt(chunkX);
		payload.WriteInt(chunkZ);

		ChunkEncoder::EncodeHeightmaps(payload, chunkData.heightmapValue);

		payload.WriteVarInt(static_cast<int32_t>(chunkData.sectionData.size()));
		payload.WriteBytes(chunkData.sectionData);

		payload.WriteVarInt(0);

		ChunkEncoder::EncodeLightData(payload);

		connection->SendRawPacket(Clientbound::Play::LevelChunkWithLight, payload);
	}

	void ChunkManager::UnloadChunk(const Ref<Connection>& connection, int32_t chunkX, int32_t chunkZ) {
		NetworkBuffer payload;
		payload.WriteInt(chunkX);
		payload.WriteInt(chunkZ);
		connection->SendRawPacket(Clientbound::Play::ForgetLevelChunk, payload);
	}

	void ChunkManager::WorkerLoop() {
		while (!m_Stopping) {
			std::function<void()> task;
			{
				std::unique_lock<std::mutex> lock(m_QueueMutex);
				m_QueueCondition.wait(lock, [this]() {
					return m_Stopping || !m_TaskQueue.empty();
				});
				if (m_Stopping && m_TaskQueue.empty()) return;
				task = std::move(m_TaskQueue.front());
				m_TaskQueue.pop();
			}
			task();
		}
	}

	void ChunkManager::SubmitTask(std::function<void()> task) {
		{
			std::lock_guard<std::mutex> lock(m_QueueMutex);
			m_TaskQueue.push(std::move(task));
		}
		m_QueueCondition.notify_one();
	}

}
