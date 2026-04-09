#include "RegionFile.h"

#include "Axiom/Core/Log.h"

#include <chrono>

namespace Axiom {

	RegionFile::RegionFile(std::filesystem::path path)
		: m_Path(std::move(path))
		, m_Offsets(ChunksPerRegion, 0)
		, m_Timestamps(ChunksPerRegion, 0) {
		EnsureOpen();
	}

	RegionFile::~RegionFile() {
		if (m_Open) {
			Flush();
			m_File.close();
		}
	}

	std::optional<std::vector<uint8_t>> RegionFile::ReadChunk(int localX, int localZ) {
		std::lock_guard lock(m_Mutex);

		if (!m_Open) {
			AX_CORE_WARN("RegionFile: cannot read chunk ({}, {}) — file not open: {}",
				localX, localZ, m_Path.string());
			return std::nullopt;
		}

		int index = ChunkIndex(localX, localZ);
		uint32_t offset = m_Offsets[index];

		if (offset == 0) {
			return std::nullopt;
		}

		uint32_t sectorOffset = (offset >> 8) & 0xFFFFFF;
		uint32_t sectorCount = offset & 0xFF;

		if (sectorCount == 0) {
			return std::nullopt;
		}

		AX_CORE_TRACE("RegionFile: reading chunk ({}, {}) from sector {} ({} sectors) in {}",
			localX, localZ, sectorOffset, sectorCount, m_Path.filename().string());

		// Stub: return empty for now — real implementation would
		// seek to sectorOffset * SectorSize, read length prefix,
		// then decompress the payload
		return std::nullopt;
	}

	void RegionFile::WriteChunk(int localX, int localZ, const std::vector<uint8_t>& data) {
		std::lock_guard lock(m_Mutex);

		if (!m_Open) {
			AX_CORE_WARN("RegionFile: cannot write chunk ({}, {}) — file not open: {}",
				localX, localZ, m_Path.string());
			return;
		}

		int index = ChunkIndex(localX, localZ);

		AX_CORE_TRACE("RegionFile: writing chunk ({}, {}) ({} bytes) to {}",
			localX, localZ, data.size(), m_Path.filename().string());

		// Stub: real implementation would allocate sectors,
		// compress, write length-prefixed data, and update
		// the offset/timestamp tables

		auto now = std::chrono::system_clock::now();
		auto epoch = std::chrono::duration_cast<std::chrono::seconds>(
			now.time_since_epoch()).count();
		m_Timestamps[index] = static_cast<uint32_t>(epoch);
	}

	void RegionFile::Flush() {
		std::lock_guard lock(m_Mutex);

		if (!m_Open) {
			return;
		}

		m_File.flush();
		AX_CORE_TRACE("RegionFile: flushed {}", m_Path.filename().string());
	}

	void RegionFile::EnsureOpen() {
		if (m_Open) {
			return;
		}

		// Create parent directories if needed
		auto parentDirectory = m_Path.parent_path();
		if (!parentDirectory.empty() && !std::filesystem::exists(parentDirectory)) {
			std::filesystem::create_directories(parentDirectory);
		}

		if (std::filesystem::exists(m_Path)) {
			m_File.open(m_Path, std::ios::in | std::ios::out | std::ios::binary);
		} else {
			// Create new region file with empty header
			m_File.open(m_Path, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
			if (m_File.good()) {
				// Write empty offset and timestamp tables
				std::vector<uint8_t> emptyHeader(HeaderSize, 0);
				m_File.write(reinterpret_cast<const char*>(emptyHeader.data()),
					static_cast<std::streamsize>(emptyHeader.size()));
				m_File.flush();
			}
		}

		m_Open = m_File.good();

		if (m_Open) {
			AX_CORE_TRACE("RegionFile: opened {}", m_Path.filename().string());
		} else {
			AX_CORE_ERROR("RegionFile: failed to open {}", m_Path.string());
		}
	}

}
