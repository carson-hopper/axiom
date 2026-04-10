#include "axpch.h"
#include "Axiom/Environment/Level/Level.h"

#include "Axiom/Core/Log.h"

#include <ranges>

namespace Axiom {

	Ref<Chunk> Level::GetOrGenerateChunk(const int32_t chunkX, const int32_t chunkZ) {
		const int64_t key = ChunkKey(chunkX, chunkZ);

		std::lock_guard<std::mutex> lock(m_ChunkMutex);
		if (const auto iterator = m_Chunks.find(key); iterator != m_Chunks.end()) {
			return iterator->second;
		}

		const auto chunkData = m_Generator->Generate(chunkX, chunkZ);
		auto chunk = CreateRef<Chunk>(chunkX, chunkZ);
		chunk->SetBiomeId(chunkData.biomeId);

		m_Chunks[key] = chunk;
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
		{
			std::lock_guard<std::mutex> lock(m_ChunkMutex);
			for (auto& chunk : m_Chunks | std::views::values) {
				if (chunk) {
					chunk->OnTick(timestep);
				}
			}
		}

		{
			std::lock_guard<std::mutex> lock(m_EntityMutex);

			std::vector<int32_t> toRemove;
			for (auto& [entityId, entity] : m_Entities) {
				if (entity && !entity->IsRemoved()) {
					entity->Tick();
				}
				if (entity && entity->IsRemoved()) {
					toRemove.push_back(entityId);
				}
			}

			for (int32_t entityId : toRemove) {
				m_Entities.erase(entityId);
			}
		}
	}

}
