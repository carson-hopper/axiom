#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Environment/Level/Chunk.h"

#include <cstdint>
#include <optional>
#include <vector>

namespace Axiom {

	/**
	 * Converts between in-memory Chunk objects
	 * and serialized byte data for persistent
	 * storage via WorldFormat.
	 */
	class ChunkSerializer {
	public:
		/**
		 * Serialize a chunk to raw bytes
		 * for storage in a region file.
		 */
		static std::vector<uint8_t> Serialize(const Chunk& chunk);

		/**
		 * Deserialize raw bytes into a
		 * Chunk object, returning empty
		 * if data is invalid.
		 */
		static std::optional<Ref<Chunk>> Deserialize(const std::vector<uint8_t>& data);
	};

}
