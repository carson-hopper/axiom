#pragma once

#include "Axiom/Core/Base.h"

#include <cstdint>
#include <fstream>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <zlib.h>

namespace Axiom {

	/**
	 * Reads chunks from Minecraft Anvil (.mca) region files.
	 *
	 * Region files contain up to 32×32 chunks. Each chunk is stored
	 * as zlib-compressed NBT data. The file header contains a 1024-entry
	 * location table mapping (chunkX % 32, chunkZ % 32) to file offsets.
	 */
	class AnvilReader {
	public:
		explicit AnvilReader(std::string worldDirectory)
			: m_WorldDirectory(std::move(worldDirectory)) {}

		/**
		 * Read the raw NBT data for a chunk at the given coordinates.
		 * Returns empty optional if the chunk doesn't exist.
		 */
		std::optional<std::vector<uint8_t>> ReadChunkNbt(const int32_t chunkX, const int32_t chunkZ) {
			const int32_t regionX = chunkX >> 5;  // Divide by 32
			const int32_t regionZ = chunkZ >> 5;
			const int32_t localX = chunkX & 31;
			const int32_t localZ = chunkZ & 31;

			std::lock_guard<std::mutex> lock(m_Mutex);

			auto& regionFile = GetOrOpenRegion(regionX, regionZ);
			if (!regionFile) return std::nullopt;

			// Read location from header
			const int headerIndex = (localX + localZ * 32) * 4;
			regionFile->seekg(headerIndex);
			uint8_t locationBytes[4];
			regionFile->read(reinterpret_cast<char*>(locationBytes), 4);

			const uint32_t location = (static_cast<uint32_t>(locationBytes[0]) << 24)
				| (static_cast<uint32_t>(locationBytes[1]) << 16)
				| (static_cast<uint32_t>(locationBytes[2]) << 8)
				| static_cast<uint32_t>(locationBytes[3]);

			if (location == 0) return std::nullopt;  // Chunk not generated

			const uint32_t sectorOffset = (location >> 8) * 4096;
			const uint32_t sectorCount = location & 0xFF;

			// Read chunk data
			regionFile->seekg(sectorOffset);

			uint8_t lengthBytes[4];
			regionFile->read(reinterpret_cast<char*>(lengthBytes), 4);
			const uint32_t dataLength = (static_cast<uint32_t>(lengthBytes[0]) << 24)
				| (static_cast<uint32_t>(lengthBytes[1]) << 16)
				| (static_cast<uint32_t>(lengthBytes[2]) << 8)
				| static_cast<uint32_t>(lengthBytes[3]);

			uint8_t compressionType;
			regionFile->read(reinterpret_cast<char*>(&compressionType), 1);

			std::vector<uint8_t> compressedData(dataLength - 1);
			regionFile->read(reinterpret_cast<char*>(compressedData.data()), dataLength - 1);

			// Decompress
			if (compressionType == 2) {
				return DecompressZlib(compressedData);
			}

			// Uncompressed or unsupported
			return compressedData;
		}

	private:
		std::unique_ptr<std::ifstream>& GetOrOpenRegion(const int32_t regionX, const int32_t regionZ) {
			const int64_t key = (static_cast<int64_t>(regionX) << 32)
				| (static_cast<int64_t>(regionZ) & 0xFFFFFFFF);

			auto iterator = m_RegionFiles.find(key);
			if (iterator != m_RegionFiles.end()) {
				return iterator->second;
			}

			const std::string path = m_WorldDirectory + "/r." + std::to_string(regionX)
				+ "." + std::to_string(regionZ) + ".mca";

			auto file = std::make_unique<std::ifstream>(path, std::ios::binary);
			if (!file->good()) {
				file.reset();
			}

			auto& stored = m_RegionFiles[key];
			stored = std::move(file);
			return stored;
		}

		static std::optional<std::vector<uint8_t>> DecompressZlib(const std::vector<uint8_t>& compressed) {
			// Start with 4x compressed size estimate
			std::vector<uint8_t> decompressed(compressed.size() * 4);

			z_stream stream{};
			stream.next_in = const_cast<uint8_t*>(compressed.data());
			stream.avail_in = static_cast<uInt>(compressed.size());

			if (inflateInit(&stream) != Z_OK) return std::nullopt;

			int result;
			do {
				stream.next_out = decompressed.data() + stream.total_out;
				stream.avail_out = static_cast<uInt>(decompressed.size() - stream.total_out);

				result = inflate(&stream, Z_NO_FLUSH);

				if (result == Z_BUF_ERROR || (result == Z_OK && stream.avail_out == 0)) {
					decompressed.resize(decompressed.size() * 2);
				}
			} while (result == Z_OK || result == Z_BUF_ERROR);

			inflateEnd(&stream);

			if (result != Z_STREAM_END) return std::nullopt;

			decompressed.resize(stream.total_out);
			return decompressed;
		}

		std::string m_WorldDirectory;
		std::mutex m_Mutex;
		std::unordered_map<int64_t, std::unique_ptr<std::ifstream>> m_RegionFiles;
	};

}
