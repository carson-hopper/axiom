#include "axpch.h"
#include "Axiom/Environment/Level/Level.h"

#include "Axiom/Core/Log.h"

#include <ranges>

namespace Axiom {

	Ref<Chunk> Level::GetOrGenerateChunk(const int32_t chunkX, const int32_t chunkZ) {
		const int64_t key = ChunkKey(chunkX, chunkZ);

		// Fast path: chunk already loaded. Most
		// reads hit this and never block on generation.
		{
			std::lock_guard<std::mutex> const lock(m_ChunkMutex);
			if (const auto iterator = m_Chunks.find(key); iterator != m_Chunks.end()) {
				return iterator->second;
			}
		}

		// Slow path: run the generator *without*
		// holding m_ChunkMutex so other threads can
		// keep reading / writing unrelated chunks
		// while this one takes tens of ms to build.
		const auto chunkData = m_Generator->Generate(chunkX, chunkZ);
		auto chunk = Ref<Chunk>::Create(chunkX, chunkZ);
		chunk->SetBiomeId(chunkData.biomeId);

		// Re-lock and re-check: another thread may
		// have raced us and inserted the same chunk
		// while we were generating. In that case,
		// discard our freshly-built copy and return
		// the winner so every caller sees one chunk.
		{
			std::lock_guard<std::mutex> const lock(m_ChunkMutex);
			if (const auto iterator = m_Chunks.find(key); iterator != m_Chunks.end()) {
				return iterator->second;
			}
			m_Chunks[key] = chunk;
		}

		return chunk;
	}

	Ref<Chunk> Level::GetChunkIfLoaded(const int32_t chunkX, const int32_t chunkZ) {
		const int64_t key = ChunkKey(chunkX, chunkZ);

		std::lock_guard<std::mutex> lock(m_ChunkMutex);
		if (const auto iterator = m_Chunks.find(key);iterator != m_Chunks.end()) {
			return iterator->second;
		}
		return nullptr;
	}

	int32_t Level::GetBlockState(const Vector3& position) {
		return GetBlockState(
			static_cast<int>(std::floor(position.x)),
			static_cast<int>(std::floor(position.y)),
			static_cast<int>(std::floor(position.z)));
	}

	int32_t Level::GetBlockState(const int x, const int y, const int z) {
		const int32_t chunkX = x >> 4;
		const int32_t chunkZ = z >> 4;
		const auto chunk = GetChunkIfLoaded(chunkX, chunkZ);
		if (!chunk) return 0;
		return chunk->GetBlockState(x, y, z);
	}

	void Level::SetBlockState(const Vector3& position, const int32_t stateId) {
		SetBlockState(
			static_cast<int>(std::floor(position.x)),
			static_cast<int>(std::floor(position.y)),
			static_cast<int>(std::floor(position.z)),
			stateId);
	}

	void Level::SetBlockState(const int x, const int y, const int z, const int32_t stateId) {
		const int32_t chunkX = x >> 4;
		const int32_t chunkZ = z >> 4;
		auto chunk = GetOrGenerateChunk(chunkX, chunkZ);
		chunk->SetBlockState(x, y, z, stateId);
	}

	void Level::AddEntity(Ref<Entity> entity) {
		std::lock_guard<std::mutex> lock(m_EntityMutex);
		m_Entities[entity->GetEntityId()] = std::move(entity);
	}

	void Level::RemoveEntity(const int32_t entityId) {
		std::lock_guard<std::mutex> lock(m_EntityMutex);
		m_Entities.erase(entityId);
	}

	Entity* Level::GetEntity(const int32_t entityId) {
		std::lock_guard<std::mutex> lock(m_EntityMutex);
		if (const auto iterator = m_Entities.find(entityId);iterator != m_Entities.end()) {
			return iterator->second.Raw();
		}
		return nullptr;
	}

	void Level::OnTick(const Timestep timestep) {
		// Snapshot chunk refs under the mutex, then tick
		// them with the lock released. A block tick that
		// calls back into SetBlockState → GetOrGenerateChunk
		// would otherwise deadlock on the non-recursive
		// m_ChunkMutex. The Ref<Chunk> copies in the
		// snapshot keep each chunk alive for the duration
		// of its tick even if another thread removes it
		// from m_Chunks in the meantime.
		std::vector<Ref<Chunk>> chunkSnapshot;
		{
			std::lock_guard<std::mutex> const lock(m_ChunkMutex);
			chunkSnapshot.reserve(m_Chunks.size());
			for (auto& chunk : m_Chunks | std::views::values) {
				if (chunk) {
					chunkSnapshot.push_back(chunk);
				}
			}
		}

		for (auto& chunk : chunkSnapshot) {
			chunk->OnTick(timestep);
		}

		// Same pattern for entities: snapshot, tick
		// without the lock, then collect any entities
		// that marked themselves as removed and erase
		// them in a second short critical section.
		std::vector<Ref<Entity>> entitySnapshot;
		{
			std::lock_guard<std::mutex> const lock(m_EntityMutex);
			entitySnapshot.reserve(m_Entities.size());
			for (auto& entity : m_Entities | std::views::values) {
				if (entity) {
					entitySnapshot.push_back(entity);
				}
			}
		}

		std::vector<int32_t> toRemove;
		for (auto& entity : entitySnapshot) {
			if (!entity->IsRemoved()) {
				entity->OnTick(timestep);
			}
			if (entity->IsRemoved()) {
				toRemove.push_back(entity->GetEntityId());
			}
		}

		if (!toRemove.empty()) {
			std::lock_guard<std::mutex> const lock(m_EntityMutex);
			for (const int32_t entityId : toRemove) {
				m_Entities.erase(entityId);
			}
		}
	}

}
