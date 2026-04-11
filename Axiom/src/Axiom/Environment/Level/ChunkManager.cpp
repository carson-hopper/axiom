#include "axpch.h"
#include "Axiom/Environment/Level/ChunkManager.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Environment/Level/ChunkEncoder.h"

#include <algorithm>

namespace Axiom {

	ChunkManager::ChunkManager(Scope<ChunkGenerator> generator, const int viewDistance)
		: m_Generator(std::move(generator))
		, m_ViewDistance(viewDistance) {

		const unsigned int threadCount = std::max(2u, std::thread::hardware_concurrency() - 1);
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

	void ChunkManager::SendInitialChunks(Ref<Connection> connection, const double playerX, const double playerZ) {
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

			const int8_t requested = connection->RequestedViewDistance();
			if (requested > 0) {
				state.effectiveViewDistance = std::clamp(
					static_cast<int>(requested), 2, m_ViewDistance);
			} else {
				state.effectiveViewDistance = m_ViewDistance;
			}
		}

		{
			const NetworkBuffer payload;
			connection->SendRawPacket(Clientbound::Play::ChunkBatchStart, payload);
		}

		QueueChunksInRadius(connection, center);
	}

	void ChunkManager::SetPlayerViewDistance(Ref<Connection> connection, int requestedDistance) {
		if (!connection) {
			return;
		}

		const int clamped = std::clamp(requestedDistance, 2, m_ViewDistance);

		ChunkPosition center{0, 0};
		bool shrinking = false;
		std::vector<ChunkPosition> distantChunks;

		{
			std::lock_guard<std::mutex> const lock(m_StateMutex);
			const auto iterator = m_PlayerStates.find(connection->Id());
			if (iterator == m_PlayerStates.end()) {
				return;
			}

			auto& state = iterator->second;
			if (state.effectiveViewDistance == clamped) {
				return;
			}

			shrinking = clamped < state.effectiveViewDistance;
			state.effectiveViewDistance = clamped;
			center = {state.lastChunkX, state.lastChunkZ};

			if (shrinking) {
				distantChunks = CollectDistantChunks(center, state);
			}
		}

		for (const auto& position : distantChunks) {
			UnloadChunk(connection, position);
		}

		if (!shrinking) {
			QueueChunksInRadius(connection, center);
		}

		AX_CORE_TRACE(
			"Per-player view distance updated: player={} distance={} (clamped from request {})",
			connection->Id().Value(), clamped, requestedDistance);
	}

	std::unordered_map<ConnectionId, int> ChunkManager::SnapshotViewDistances() const {
		std::unordered_map<ConnectionId, int> result;
		std::lock_guard<std::mutex> const lock(m_StateMutex);
		result.reserve(m_PlayerStates.size());
		for (const auto& [id, state] : m_PlayerStates) {
			result.emplace(id, state.effectiveViewDistance);
		}
		return result;
	}

	void ChunkManager::OnPlayerMove(const Ref<Connection>& connection, const double playerX, const double playerZ) {
		const ChunkPosition center{BlockToChunk(playerX), BlockToChunk(playerZ)};

		std::vector<ChunkPosition> distantChunks;

		{
			std::lock_guard<std::mutex> const lock(m_StateMutex);
			const auto iterator = m_PlayerStates.find(connection->Id());
			if (iterator == m_PlayerStates.end()) {
				return;
			}

			auto& state = iterator->second;
			if (center.x == state.lastChunkX && center.z == state.lastChunkZ) {
				return;
			}

			state.lastChunkX = center.x;
			state.lastChunkZ = center.z;

			distantChunks = CollectDistantChunks(center, state);
		}

		for (const auto& position : distantChunks) {
			UnloadChunk(connection, position);
		}

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
		// Save any dirty chunks loaded by this player before removal
		{
			std::lock_guard<std::mutex> lock(m_StateMutex);
			auto iterator = m_PlayerStates.find(connectionId);
			if (iterator != m_PlayerStates.end()) {
				std::lock_guard<std::mutex> dirtyLock(m_DirtyMutex);
				for (const auto& chunkPos : iterator->second.loadedChunks) {
					int64_t key = ChunkKey(chunkPos.x, chunkPos.z);
					if (m_DirtyChunks.contains(key)) {
						m_DirtyChunks.erase(key);
						if (m_ChunkUnloadCallback) {
							m_ChunkUnloadCallback(chunkPos);
						}
					}
				}
			}
		}

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

			const int playerDistance = state.effectiveViewDistance;

			for (int radius = 0; radius <= playerDistance; radius++) {
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

		std::sort(toGenerate.begin(), toGenerate.end(),
			[&centerPosition](const ChunkPosition& positionA, const ChunkPosition& positionB) {
				int distanceA = (positionA.x - centerPosition.x) * (positionA.x - centerPosition.x)
					+ (positionA.z - centerPosition.z) * (positionA.z - centerPosition.z);
				int distanceB = (positionB.x - centerPosition.x) * (positionB.x - centerPosition.x)
					+ (positionB.z - centerPosition.z) * (positionB.z - centerPosition.z);
				return distanceA < distanceB;
			});

		auto remaining = std::make_shared<std::atomic<int>>(static_cast<int>(toGenerate.size()));
		const int totalChunks = static_cast<int>(toGenerate.size());

		for (const auto& chunkPosition : toGenerate) {
			auto connectionRef = connection;

			SubmitTask([this, connectionRef, chunkPosition, remaining, totalChunks]() mutable {
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

	std::vector<ChunkPosition> ChunkManager::CollectDistantChunks(
		const ChunkPosition& centerPosition, PlayerChunkState& state) {

		const int playerDistance = state.effectiveViewDistance;
		std::vector<ChunkPosition> toUnload;

		for (const auto& position : state.loadedChunks) {
			if (std::abs(position.x - centerPosition.x) > playerDistance + 1 ||
				std::abs(position.z - centerPosition.z) > playerDistance + 1) {
				toUnload.push_back(position);
			}
		}

		for (const auto& position : toUnload) {
			state.loadedChunks.erase(position);
		}

		return toUnload;
	}

	int32_t ChunkManager::GetBlockAt(const int32_t worldX, const int32_t worldY, const int32_t worldZ) const {
		const int32_t chunkX = worldX >> 4;
		const int32_t chunkZ = worldZ >> 4;
		const int64_t key = ChunkKey(chunkX, chunkZ);

		std::lock_guard<std::mutex> lock(m_ChunkCacheMutex);
		const auto iterator = m_ChunkCache.find(key);
		if (iterator == m_ChunkCache.end()) return 0;

		return iterator->second->GetBlockState(worldX, worldY, worldZ);
	}

	void ChunkManager::CacheChunkBlocks(const int32_t chunkX, const int32_t chunkZ) {
		const int64_t key = ChunkKey(chunkX, chunkZ);

		{
			std::lock_guard<std::mutex> lock(m_ChunkCacheMutex);
			if (m_ChunkCache.contains(key)) return;
		}

		auto chunk = Ref<Chunk>::Create(chunkX, chunkZ);
		const int baseX = chunkX * 16;
		const int baseZ = chunkZ * 16;

		for (int localX = 0; localX < 16; localX++) {
			for (int localZ = 0; localZ < 16; localZ++) {
				for (int worldY = -64; worldY <= 319; worldY++) {
					const int32_t state = m_Generator->GetBlockAt(
						baseX + localX, worldY, baseZ + localZ);
					if (state != 0) {
						chunk->SetBlockState(localX, worldY, localZ, state);
					}
				}
			}
		}

		std::lock_guard<std::mutex> lock(m_ChunkCacheMutex);
		m_ChunkCache[key] = std::move(chunk);
	}

	void ChunkManager::SendChunk(Ref<Connection> connection, const ChunkPosition chunkPosition) {
		const auto chunkData = m_Generator->Generate(chunkPosition.x, chunkPosition.z);

		NetworkBuffer payload;
		payload.WriteInt(chunkPosition.x);
		payload.WriteInt(chunkPosition.z);

		ChunkEncoder::EncodeHeightmaps(payload, chunkData.heightmapValue);

		payload.WriteVarInt(static_cast<int32_t>(chunkData.sectionData.size()));
		payload.WriteBytes(chunkData.sectionData);

		payload.WriteVarInt(0);

		ChunkEncoder::EncodeLightData(payload, chunkData.skyLight);

		connection->SendRawPacket(Clientbound::Play::LevelChunkWithLight, payload);

		if (m_ChunkSentCallback) {
			m_ChunkSentCallback(chunkPosition);
		}
	}

	void ChunkManager::UnloadChunk(Ref<Connection> connection, const ChunkPosition chunkPosition) {
		NetworkBuffer payload;
		// ForgetLevelChunk uses a packed long: x in low 32 bits, z in high 32 bits
		int64_t packed = (static_cast<int64_t>(chunkPosition.x) & 0xFFFFFFFFL)
			| ((static_cast<int64_t>(chunkPosition.z) & 0xFFFFFFFFL) << 32);
		payload.WriteLong(packed);
		connection->SendRawPacket(Clientbound::Play::ForgetLevelChunk, payload);

		// Save dirty chunk on unload
		int64_t key = ChunkKey(chunkPosition.x, chunkPosition.z);
		{
			std::lock_guard<std::mutex> lock(m_DirtyMutex);
			if (m_DirtyChunks.contains(key)) {
				m_DirtyChunks.erase(key);
				if (m_ChunkUnloadCallback) {
					m_ChunkUnloadCallback(chunkPosition);
				}
			}
		}
	}

	void ChunkManager::MarkChunkDirty(int32_t chunkX, int32_t chunkZ) {
		std::lock_guard<std::mutex> lock(m_DirtyMutex);
		m_DirtyChunks.insert(ChunkKey(chunkX, chunkZ));
	}

	void ChunkManager::SaveAllDirtyChunks() {
		// Snapshot the dirty-chunk keys under the lock
		// and clear the set, then release the lock before
		// invoking the unload callback. The callback
		// writes to the region file (potentially slow disk
		// I/O) and may itself call back into ChunkManager
		// — holding m_DirtyMutex across either would
		// serialise every other dirty-marking operation
		// behind shutdown saves and risk re-entrant
		// deadlock.
		std::vector<ChunkPosition> positions;
		{
			std::lock_guard<std::mutex> const lock(m_DirtyMutex);
			positions.reserve(m_DirtyChunks.size());
			for (const int64_t key : m_DirtyChunks) {
				const auto chunkX = static_cast<int32_t>(key >> 32);
				const auto chunkZ = static_cast<int32_t>(key & 0xFFFFFFFF);
				positions.push_back({chunkX, chunkZ});
			}
			m_DirtyChunks.clear();
		}

		if (m_ChunkUnloadCallback) {
			for (const auto& position : positions) {
				m_ChunkUnloadCallback(position);
			}
		}

		AX_CORE_INFO("Saved {} dirty chunks", positions.size());
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
