#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Data/Format/RegionFile.h"

#include <filesystem>
#include <mutex>
#include <string>
#include <unordered_map>

namespace Axiom {

	/**
	 * Manages a cache of open region files.
	 * Region files are opened on demand and
	 * kept in an LRU-style cache to avoid
	 * excessive file handle usage.
	 */
	class RegionFileStorage {
	public:
		static constexpr size_t MaxOpenFiles = 256;

		explicit RegionFileStorage(std::filesystem::path regionDirectory);
		~RegionFileStorage();

		RegionFileStorage(const RegionFileStorage&) = delete;
		RegionFileStorage& operator=(const RegionFileStorage&) = delete;

		/**
		 * Get or open the region file that
		 * contains the given chunk.
		 */
		Ref<RegionFile> GetRegionFile(int32_t chunkX, int32_t chunkZ);

		/**
		 * Flush all open region files
		 * to disk.
		 */
		void FlushAll();

		/**
		 * Close all open region files
		 * and clear the cache.
		 */
		void CloseAll();

		size_t OpenFileCount() const;

	private:
		static int64_t RegionKey(int32_t regionX, int32_t regionZ) {
			return (static_cast<int64_t>(regionX) << 32) | (static_cast<int64_t>(regionZ) & 0xFFFFFFFF);
		}

		std::filesystem::path m_RegionDirectory;
		mutable std::mutex m_Mutex;
		std::unordered_map<int64_t, Ref<RegionFile>> m_RegionFiles;
	};

}
