#include "RegionFileStorage.h"

#include "Axiom/Core/Log.h"

#include <sstream>

namespace Axiom {

	RegionFileStorage::RegionFileStorage(std::filesystem::path regionDirectory)
		: m_RegionDirectory(std::move(regionDirectory)) {
		if (!std::filesystem::exists(m_RegionDirectory)) {
			std::filesystem::create_directories(m_RegionDirectory);
		}
		AX_CORE_INFO("RegionFileStorage: using directory {}", m_RegionDirectory.string());
	}

	RegionFileStorage::~RegionFileStorage() {
		CloseAll();
	}

	Ref<RegionFile> RegionFileStorage::GetRegionFile(int32_t chunkX, int32_t chunkZ) {
		int32_t regionX = RegionFile::ChunkToRegion(chunkX);
		int32_t regionZ = RegionFile::ChunkToRegion(chunkZ);
		int64_t key = RegionKey(regionX, regionZ);

		std::lock_guard lock(m_Mutex);

		auto iterator = m_RegionFiles.find(key);
		if (iterator != m_RegionFiles.end()) {
			return iterator->second;
		}

		// Evict oldest if at capacity
		if (m_RegionFiles.size() >= MaxOpenFiles) {
			AX_CORE_WARN("RegionFileStorage: cache full ({} files), evicting oldest",
				m_RegionFiles.size());
			auto oldest = m_RegionFiles.begin();
			oldest->second->Flush();
			m_RegionFiles.erase(oldest);
		}

		// Build filename: r.X.Z.mca
		std::ostringstream filename;
		filename << "r." << regionX << "." << regionZ << ".mca";
		auto path = m_RegionDirectory / filename.str();

		auto regionFile = CreateRef<RegionFile>(path);
		m_RegionFiles[key] = regionFile;

		AX_CORE_TRACE("RegionFileStorage: opened region ({}, {}) — {} files cached",
			regionX, regionZ, m_RegionFiles.size());

		return regionFile;
	}

	void RegionFileStorage::FlushAll() {
		std::lock_guard lock(m_Mutex);

		for (auto& [key, regionFile] : m_RegionFiles) {
			regionFile->Flush();
		}

		AX_CORE_INFO("RegionFileStorage: flushed {} region files", m_RegionFiles.size());
	}

	void RegionFileStorage::CloseAll() {
		std::lock_guard lock(m_Mutex);

		size_t count = m_RegionFiles.size();
		m_RegionFiles.clear();

		if (count > 0) {
			AX_CORE_INFO("RegionFileStorage: closed {} region files", count);
		}
	}

	size_t RegionFileStorage::OpenFileCount() const {
		std::lock_guard lock(m_Mutex);
		return m_RegionFiles.size();
	}

}
