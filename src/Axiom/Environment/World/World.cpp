#include "World.h"

#include "Axiom/Core/Log.h"

namespace Axiom {

	Ref<Chunk> World::GetOrGenerateChunk(int32_t chunkX, int32_t chunkZ) {
		const int64_t key = ChunkKey(chunkX, chunkZ);

		std::lock_guard<std::mutex> lock(m_ChunkMutex);
		if (const auto iterator = m_Chunks.find(key);iterator != m_Chunks.end()) {
			return iterator->second;
		}

		const auto chunkData = m_Generator->Generate(chunkX, chunkZ);
		auto chunk = CreateRef<Chunk>(chunkX, chunkZ);
		chunk->SetBiomeId(chunkData.biomeId);

		m_Chunks[key] = chunk;
		return chunk;
	}

	Ref<Chunk> World::GetChunkIfLoaded(const int32_t chunkX, const int32_t chunkZ) {
		const int64_t key = ChunkKey(chunkX, chunkZ);

		std::lock_guard<std::mutex> lock(m_ChunkMutex);
		if (const auto iterator = m_Chunks.find(key);iterator != m_Chunks.end()) {
			return iterator->second;
		}
		return nullptr;
	}

	int32_t World::GetBlockState(const Vector3& position) {
		return GetBlockState(
			static_cast<int>(std::floor(position.x)),
			static_cast<int>(std::floor(position.y)),
			static_cast<int>(std::floor(position.z)));
	}

	int32_t World::GetBlockState(const int x, const int y, const int z) {
		const int32_t chunkX = x >> 4;
		const int32_t chunkZ = z >> 4;
		const auto chunk = GetChunkIfLoaded(chunkX, chunkZ);
		if (!chunk) return 0;
		return chunk->GetBlockState(x, y, z);
	}

	void World::SetBlockState(const Vector3& position, const int32_t stateId) {
		SetBlockState(
			static_cast<int>(std::floor(position.x)),
			static_cast<int>(std::floor(position.y)),
			static_cast<int>(std::floor(position.z)),
			stateId);
	}

	void World::SetBlockState(const int x, const int y, const int z, const int32_t stateId) {
		const int32_t chunkX = x >> 4;
		const int32_t chunkZ = z >> 4;
		const auto chunk = GetOrGenerateChunk(chunkX, chunkZ);
		chunk->SetBlockState(x, y, z, stateId);
	}

	void World::AddEntity(Ref<Entity> entity) {
		std::lock_guard<std::mutex> lock(m_EntityMutex);
		m_Entities[entity->EntityId()] = std::move(entity);
	}

	void World::RemoveEntity(const int32_t entityId) {
		std::lock_guard<std::mutex> lock(m_EntityMutex);
		m_Entities.erase(entityId);
	}

	Entity* World::GetEntity(const int32_t entityId) {
		std::lock_guard<std::mutex> lock(m_EntityMutex);
		const auto iterator = m_Entities.find(entityId);
		if (iterator != m_Entities.end()) {
			return iterator->second.get();
		}
		return nullptr;
	}

	void World::Tick() {
		std::lock_guard<std::mutex> lock(m_EntityMutex);

		std::vector<int32_t> toRemove;
		for (auto& [entityId, entity] : m_Entities) {
			entity->Tick();
			if (entity->IsRemoved()) {
				toRemove.push_back(entityId);
			}
		}

		for (int32_t entityId : toRemove) {
			m_Entities.erase(entityId);
		}
	}

}
