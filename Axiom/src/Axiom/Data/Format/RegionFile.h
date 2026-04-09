#pragma once

#include "Axiom/Core/Base.h"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <optional>
#include <vector>

namespace Axiom {

	/**
	 * Handles a single .mca region file using
	 * the Anvil format with 4KiB sector
	 * allocation for chunk storage.
	 */
	class RegionFile {
	public:
		static constexpr int SectorSize = 4096;
		static constexpr int ChunksPerRegion = 32 * 32;
		static constexpr int HeaderSize = SectorSize * 2;

		explicit RegionFile(std::filesystem::path path);
		~RegionFile();

		RegionFile(const RegionFile&) = delete;
		RegionFile& operator=(const RegionFile&) = delete;

		/**
		 * Read chunk data from the region file.
		 * Local coordinates are 0-31 within
		 * the region.
		 */
		std::optional<std::vector<uint8_t>> ReadChunk(int localX, int localZ);

		/**
		 * Write chunk data to the region file.
		 * Allocates sectors as needed using
		 * 4KiB alignment.
		 */
		void WriteChunk(int localX, int localZ, const std::vector<uint8_t>& data);

		/**
		 * Flush any pending writes
		 * to disk.
		 */
		void Flush();

		/**
		 * Whether the region file has been
		 * successfully opened.
		 */
		bool IsOpen() const { return m_Open; }

		const std::filesystem::path& Path() const { return m_Path; }

		/**
		 * Convert world chunk coordinates to
		 * region file coordinates.
		 */
		static int ChunkToRegion(int chunkCoord) {
			return chunkCoord >> 5;
		}

		/**
		 * Convert world chunk coordinates to
		 * local region coordinates.
		 */
		static int ChunkToLocal(int chunkCoord) {
			return chunkCoord & 31;
		}

	private:
		int ChunkIndex(int localX, int localZ) const {
			return (localX & 31) + (localZ & 31) * 32;
		}

		void EnsureOpen();

		std::filesystem::path m_Path;
		std::fstream m_File;
		std::mutex m_Mutex;
		bool m_Open = false;

		std::vector<uint32_t> m_Offsets;
		std::vector<uint32_t> m_Timestamps;
	};

}
