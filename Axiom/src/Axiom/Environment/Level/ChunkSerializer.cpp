#include "ChunkSerializer.h"

#include "Axiom/Core/Log.h"

namespace Axiom {

	std::vector<uint8_t> ChunkSerializer::Serialize(const Chunk& chunk) {
		AX_CORE_TRACE("ChunkSerializer: serializing chunk ({}, {})",
			chunk.ChunkX(), chunk.ChunkZ());

		// Stub: real implementation would serialize section
		// palettes, block states, biomes, heightmaps, and
		// block entity NBT into the Anvil chunk format
		std::vector<uint8_t> data;
		return data;
	}

	std::optional<Ref<Chunk>> ChunkSerializer::Deserialize(const std::vector<uint8_t>& data) {
		if (data.empty()) {
			AX_CORE_WARN("ChunkSerializer: cannot deserialize empty data");
			return std::nullopt;
		}

		AX_CORE_TRACE("ChunkSerializer: deserializing chunk ({} bytes)", data.size());

		// Stub: real implementation would parse Anvil chunk
		// format, rebuild section palettes, block states,
		// biomes, heightmaps, and block entities
		return std::nullopt;
	}

}
