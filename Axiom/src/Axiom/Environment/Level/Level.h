#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Core/Math.h"
#include "Axiom/Core/Tickable.h"
#include "Axiom/Environment/Level/Chunk.h"
#include "Generator/ChunkGenerator.h"
#include "Axiom/Environment/Entity/Entity.h"

#include <mutex>
#include <string>
#include <unordered_map>

namespace Axiom {

	/**
	 * Represents a dimension/world containing chunks and entities.
	 * Ticks all loaded chunks and entities each game tick.
	 */
	class Level : public Tickable, public virtual RefCounted {
	public:
		Level(std::string name, Scope<ChunkGenerator> generator)
			: m_Name(std::move(name))
			, m_Generator(std::move(generator)) {}

		/**
		 * Called every game tick (50ms interval, 20 TPS).
		 * Ticks all loaded chunks and entities.
		 */
		void OnTick(Timestep timestep) override;

		const std::string& Name() const { return m_Name; }

		// ----- Chunks ---------------------------------------------------

		/**
		 * Get a chunk, generating it if it doesn't exist.
		 */
		Ref<Chunk> GetOrGenerateChunk(int32_t chunkX, int32_t chunkZ);

		/**
		 * Get a chunk if it's already loaded, nullptr otherwise.
		 */
		Ref<Chunk> GetChunkIfLoaded(int32_t chunkX, int32_t chunkZ);

		/**
		 * Get a block state at world coordinates.
		 */
		int32_t GetBlockState(const Vector3& position);
		int32_t GetBlockState(int x, int y, int z);

		/**
		 * Set a block state at world coordinates.
		 */
		void SetBlockState(const Vector3& position, int32_t stateId);
		void SetBlockState(int x, int y, int z, int32_t stateId);

		// ----- Entities -------------------------------------------------

		void AddEntity(Ref<Entity> entity);
		void RemoveEntity(int32_t entityId);
		Entity* GetEntity(int32_t entityId);

		[[nodiscard]]
		ChunkGenerator& Generator() { return *m_Generator; }
		const ChunkGenerator& Generator() const { return *m_Generator; }

	private:
		static int64_t ChunkKey(const int32_t chunkX, const int32_t chunkZ) {
			return (static_cast<int64_t>(chunkX) << 32) | (static_cast<int64_t>(chunkZ) & 0xFFFFFFFF);
		}

		std::string m_Name;
		/**
		 * Sole owner of this level's generator. Per
		 * STYLE.md §4.3, Scope<T> is the default for
		 * owning pointers — Ref<T> would imply shared
		 * ownership, which no other subsystem takes
		 * here. Observers go through Generator() which
		 * returns a plain reference.
		 */
		Scope<ChunkGenerator> m_Generator;

		std::mutex m_ChunkMutex;
		std::unordered_map<int64_t, Ref<Chunk>> m_Chunks;

		std::mutex m_EntityMutex;
		std::unordered_map<int32_t, Ref<Entity>> m_Entities;
	};

}
