#include "axpch.h"
#include "Axiom/Environment/Level/ChunkManager.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Environment/Level/ChunkEncoder.h"

#include <algorithm>

namespace Axiom {

	ChunkManager::ChunkManager(Ref<ChunkGenerator> generator, const int viewDistance)
		: m_Generator(std::move(generator))
		, m_ViewDistance(viewDistance) {

		const unsigned int threadCount = std::max(2u, std::thread::hardware_concurrency() / 2);
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

	void ChunkManager::SendInitialChunks(const Ref<Connection> &connection, const double playerX, const double playerZ) {
		const ChunkPosition center{BlockToChunk(playerX), BlockToChunk(playerZ)};

		{
			NetworkBuffer payload;
			payload.WriteVarInt(center.x);
			payload.WriteVarInt(center.z);
			connection->SendRawPacket(Clientbound::Play::SetChunkCacheCenter, payload);
		}

		{
			std::lock_guard<std::mutex> lock(m_StateMutex);
			auto& state = m_PlayerStates[connection->Id()];
			state.lastChunkX = center.x;
			state.lastChunkZ = center.z;
		}

		{
			const NetworkBuffer payload;
			connection->SendRawPacket(Clientbound::Play::ChunkBatchStart, payload);
		}

		QueueChunksInRadius(connection, center);
	}

	void ChunkManager::OnPlayerMove(Ref<Connection> connection, const double playerX, const double playerZ) {
		const ChunkPosition center{BlockToChunk(playerX), BlockToChunk(playerZ)};

		bool needsUpdate = false;

		{
			std::lock_guard<std::mutex> lock(m_StateMutex);
			const auto iterator = m_PlayerStates.find(connection->Id());
			if (iterator == m_PlayerStates.end()) return;

			auto& state = iterator->second;
			if (center.x == state.lastChunkX && center.z == state.lastChunkZ) return;

			state.lastChunkX = center.x;
			state.lastChunkZ = center.z;
			needsUpdate = true;

			UnloadDistantChunks(connection, center, state);
		}

		if (!needsUpdate) return;

		{
			NetworkBuffer payload;
			payload.WriteVarInt(center.x);
			payload.WriteVarInt(center.z);
			connection->SendRawPacket(Clientbound::Play::SetChunkCacheCenter, payload);
		}

		{
			const NetworkBuffer payload;
			connection->SendRawPacket(Clientbound::Play::ChunkBatchStart, payload);
		}

		QueueChunksInRadius(connection, center);
	}

	void ChunkManager::RemovePlayer(ConnectionId connectionId) {
		std::lock_guard<std::mutex> lock(m_StateMutex);
		m_PlayerStates.erase(connectionId);
	}

	void ChunkManager::QueueChunksInRadius(Ref<Connection> connection, const ChunkPosition centerPosition) {
		std::vector<ChunkPosition> toGenerate;

		{
			std::lock_guard<std::mutex> lock(m_StateMutex);
			const auto stateIterator = m_PlayerStates.find(connection->Id());
			if (stateIterator == m_PlayerStates.end()) return;
			auto& state = stateIterator->second;

			for (int radius = 0; radius <= m_ViewDistance; radius++) {
				for (int offsetX = -radius; offsetX <= radius; offsetX++) {
					for (int offsetZ = -radius; offsetZ <= radius; offsetZ++) {
						if (std::abs(offsetX) != radius && std::abs(offsetZ) != radius) continue;

						const ChunkPosition position{centerPosition.x + offsetX, centerPosition.z + offsetZ};
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
		const int totalChunks = static_cast<int>(toGenerate.size());

		for (const auto& chunkPosition : toGenerate) {
			auto connectionRef = connection;

			SubmitTask([this, connectionRef, chunkPosition, remaining, totalChunks]() {
				if (!connectionRef->IsConnected()) return;

				SendChunk(connectionRef, chunkPosition);

				if (remaining->fetch_sub(1) == 1) {
					NetworkBuffer payload;
					payload.WriteVarInt(totalChunks);
					connectionRef->SendRawPacket(Clientbound::Play::ChunkBatchFinished, payload);
				}
			});
		}
	}

	void ChunkManager::UnloadDistantChunks(Ref<Connection> connection, const ChunkPosition centerPosition,
		PlayerChunkState& state) {

		std::vector<ChunkPosition> toUnload;

		for (const auto& position : state.loadedChunks) {
			if (std::abs(position.x - centerPosition.x) > m_ViewDistance + 1 ||
				std::abs(position.z - centerPosition.z) > m_ViewDistance + 1) {
				toUnload.push_back(position);
			}
		}

		for (const auto& position : toUnload) {
			UnloadChunk(connection, position);
			state.loadedChunks.erase(position);
		}
	}

	void ChunkManager::SendChunk(const Ref<Connection>& connection, const ChunkPosition chunkPosition) const {
		const auto chunkData = m_Generator->Generate(chunkPosition.x, chunkPosition.z);

		NetworkBuffer payload;
		payload.WriteInt(chunkPosition.x);
		payload.WriteInt(chunkPosition.z);

		ChunkEncoder::EncodeHeightmaps(payload, chunkData.heightmapValue);

		payload.WriteVarInt(static_cast<int32_t>(chunkData.sectionData.size()));
		payload.WriteBytes(chunkData.sectionData);

		payload.WriteVarInt(0);

		ChunkEncoder::EncodeLightData(payload);

		connection->SendRawPacket(Clientbound::Play::LevelChunkWithLight, payload);

		if (m_ChunkSentCallback) {
			m_ChunkSentCallback(chunkPosition);
		}
	}

	void ChunkManager::UnloadChunk(const Ref<Connection>& connection, const ChunkPosition chunkPosition) {
		NetworkBuffer payload;
		payload.WriteInt(chunkPosition.x);
		payload.WriteInt(chunkPosition.z);
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
