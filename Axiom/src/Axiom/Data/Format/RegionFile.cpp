#include "RegionFile.h"

#include "Axiom/Core/Log.h"

#include <zlib.h>

#include <array>
#include <chrono>
#include <cstring>

namespace Axiom {

	static uint32_t ReadBigEndianU32(const uint8_t* bytes) {
		return (static_cast<uint32_t>(bytes[0]) << 24)
			| (static_cast<uint32_t>(bytes[1]) << 16)
			| (static_cast<uint32_t>(bytes[2]) << 8)
			| static_cast<uint32_t>(bytes[3]);
	}

	static void WriteBigEndianU32(uint8_t* bytes, uint32_t value) {
		bytes[0] = static_cast<uint8_t>((value >> 24) & 0xFFu);
		bytes[1] = static_cast<uint8_t>((value >> 16) & 0xFFu);
		bytes[2] = static_cast<uint8_t>((value >> 8) & 0xFFu);
		bytes[3] = static_cast<uint8_t>(value & 0xFFu);
	}

	// Decompression-bomb guard — above any realistic chunk payload.
	static constexpr size_t MaxInflatedChunkBytes = 16 * 1024 * 1024;

	static std::optional<std::vector<uint8_t>> InflateZlib(
		const std::vector<uint8_t>& compressed) {

		z_stream stream{};
		if (inflateInit(&stream) != Z_OK) {
			return std::nullopt;
		}

		stream.next_in = const_cast<Bytef*>(compressed.data());
		stream.avail_in = static_cast<uInt>(compressed.size());

		std::vector<uint8_t> output(
			std::min(compressed.size() * 4, MaxInflatedChunkBytes));

		while (true) {
			stream.next_out = output.data() + stream.total_out;
			stream.avail_out = static_cast<uInt>(output.size() - stream.total_out);

			const int result = inflate(&stream, Z_NO_FLUSH);
			if (result == Z_STREAM_END) {
				break;
			}
			if (result != Z_OK) {
				inflateEnd(&stream);
				return std::nullopt;
			}
			if (stream.avail_out == 0) {
				if (output.size() >= MaxInflatedChunkBytes) {
					inflateEnd(&stream);
					return std::nullopt;
				}
				output.resize(
					std::min(output.size() * 2, MaxInflatedChunkBytes));
			}
		}

		output.resize(stream.total_out);
		inflateEnd(&stream);
		return output;
	}

	static std::optional<std::vector<uint8_t>> DeflateZlib(
		const std::vector<uint8_t>& data) {

		std::vector<uint8_t> output(compressBound(static_cast<uLong>(data.size())));
		uLongf size = static_cast<uLongf>(output.size());
		if (compress2(output.data(), &size, data.data(),
				static_cast<uLong>(data.size()), Z_DEFAULT_COMPRESSION) != Z_OK) {
			return std::nullopt;
		}
		output.resize(size);
		return output;
	}

	// ---- RegionFile implementation -----------------------------------

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

		const int index = ChunkIndex(localX, localZ);
		const uint32_t location = m_Offsets[index];
		if (location == 0) {
			return std::nullopt;
		}

		const uint32_t sectorOffset = (location >> 8) & 0xFFFFFFu;
		const uint32_t sectorCount = location & 0xFFu;
		if (sectorOffset < 2 || sectorCount == 0) {
			AX_CORE_WARN("RegionFile: bogus location for chunk ({}, {}) in {}: offset={} count={}",
				localX, localZ, m_Path.filename().string(), sectorOffset, sectorCount);
			return std::nullopt;
		}

		const auto byteOffset = static_cast<std::streamoff>(sectorOffset)
			* static_cast<std::streamoff>(SectorSize);
		m_File.seekg(byteOffset);
		if (!m_File.good()) {
			m_File.clear();
			AX_CORE_WARN("RegionFile: seek failed for chunk ({}, {}) in {}",
				localX, localZ, m_Path.filename().string());
			return std::nullopt;
		}

		std::array<uint8_t, 5> header{};
		m_File.read(reinterpret_cast<char*>(header.data()), header.size());
		if (static_cast<size_t>(m_File.gcount()) != header.size()) {
			m_File.clear();
			AX_CORE_WARN("RegionFile: short read on chunk header ({}, {}) in {}",
				localX, localZ, m_Path.filename().string());
			return std::nullopt;
		}

		const uint32_t dataLength = ReadBigEndianU32(header.data());
		const uint8_t compressionType = header[4];

		if (dataLength <= 1 || dataLength > sectorCount * SectorSize) {
			AX_CORE_WARN("RegionFile: bogus length {} for chunk ({}, {}) in {} (sectors={})",
				dataLength, localX, localZ, m_Path.filename().string(), sectorCount);
			return std::nullopt;
		}

		std::vector<uint8_t> compressed(dataLength - 1);
		m_File.read(reinterpret_cast<char*>(compressed.data()),
			static_cast<std::streamsize>(compressed.size()));
		if (static_cast<size_t>(m_File.gcount()) != compressed.size()) {
			m_File.clear();
			AX_CORE_WARN("RegionFile: short read on chunk payload ({}, {}) in {}",
				localX, localZ, m_Path.filename().string());
			return std::nullopt;
		}

		switch (compressionType) {
			case 2: return InflateZlib(compressed);
			case 3: return compressed;
			default:
				AX_CORE_WARN("RegionFile: unsupported compression type {} for chunk ({}, {}) in {}",
					compressionType, localX, localZ, m_Path.filename().string());
				return std::nullopt;
		}
	}

	void RegionFile::WriteChunk(int localX, int localZ, const std::vector<uint8_t>& data) {
		std::lock_guard lock(m_Mutex);

		if (!m_Open) {
			AX_CORE_WARN("RegionFile: cannot write chunk ({}, {}) — file not open: {}",
				localX, localZ, m_Path.string());
			return;
		}

		const auto compressed = DeflateZlib(data);
		if (!compressed) {
			AX_CORE_ERROR("RegionFile: failed to compress chunk ({}, {}) for {}",
				localX, localZ, m_Path.filename().string());
			return;
		}

		const size_t totalBytes = 5 + compressed->size();
		const uint32_t sectorsNeeded = static_cast<uint32_t>(
			(totalBytes + SectorSize - 1) / SectorSize);
		if (sectorsNeeded > 0xFFu) {
			// Anvil caps in-region chunks at ~1 MiB via its 1-byte
			// sectorCount field. External .mcc overflow is future work.
			AX_CORE_ERROR(
				"RegionFile: chunk ({}, {}) needs {} sectors (>{}); dropped",
				localX, localZ, sectorsNeeded, 0xFF);
			return;
		}

		const int index = ChunkIndex(localX, localZ);
		const uint32_t oldLocation = m_Offsets[index];
		const uint32_t oldSectorOffset = (oldLocation >> 8) & 0xFFFFFFu;
		const uint32_t oldSectorCount = oldLocation & 0xFFu;

		uint32_t targetSectorOffset = 0;
		if (oldLocation != 0 && oldSectorCount >= sectorsNeeded) {
			targetSectorOffset = oldSectorOffset;
		} else {
			if (oldLocation != 0) {
				FreeSectors(oldSectorOffset, oldSectorCount);
			}
			targetSectorOffset = AllocateSectors(sectorsNeeded);
		}

		const auto byteOffset = static_cast<std::streamoff>(targetSectorOffset)
			* static_cast<std::streamoff>(SectorSize);
		m_File.seekp(byteOffset);
		if (!m_File.good()) {
			m_File.clear();
			AX_CORE_ERROR("RegionFile: seek failed for chunk ({}, {}) in {}",
				localX, localZ, m_Path.filename().string());
			return;
		}

		const uint32_t dataLength = static_cast<uint32_t>(1 + compressed->size());
		std::array<uint8_t, 5> header{};
		WriteBigEndianU32(header.data(), dataLength);
		header[4] = 2; // zlib

		m_File.write(reinterpret_cast<const char*>(header.data()),
			static_cast<std::streamsize>(header.size()));
		m_File.write(reinterpret_cast<const char*>(compressed->data()),
			static_cast<std::streamsize>(compressed->size()));

		const size_t padding =
			(static_cast<size_t>(sectorsNeeded) * SectorSize) - totalBytes;
		if (padding > 0) {
			const std::vector<uint8_t> zeros(padding, 0);
			m_File.write(reinterpret_cast<const char*>(zeros.data()),
				static_cast<std::streamsize>(padding));
		}

		m_Offsets[index] = (targetSectorOffset << 8) | (sectorsNeeded & 0xFFu);

		const auto now = std::chrono::duration_cast<std::chrono::seconds>(
			std::chrono::system_clock::now().time_since_epoch()).count();
		m_Timestamps[index] = static_cast<uint32_t>(now);

		std::array<uint8_t, 4> entry{};

		WriteBigEndianU32(entry.data(), m_Offsets[index]);
		m_File.seekp(static_cast<std::streamoff>(index) * 4);
		m_File.write(reinterpret_cast<const char*>(entry.data()),
			static_cast<std::streamsize>(entry.size()));

		WriteBigEndianU32(entry.data(), m_Timestamps[index]);
		m_File.seekp(SectorSize + (static_cast<std::streamoff>(index) * 4));
		m_File.write(reinterpret_cast<const char*>(entry.data()),
			static_cast<std::streamsize>(entry.size()));

		if (!m_File.good()) {
			m_File.clear();
			AX_CORE_ERROR("RegionFile: write failed for chunk ({}, {}) in {}",
				localX, localZ, m_Path.filename().string());
		}
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

		auto parentDirectory = m_Path.parent_path();
		if (!parentDirectory.empty() && !std::filesystem::exists(parentDirectory)) {
			std::filesystem::create_directories(parentDirectory);
		}

		const bool existed = std::filesystem::exists(m_Path);
		if (existed) {
			m_File.open(m_Path, std::ios::in | std::ios::out | std::ios::binary);
		} else {
			m_File.open(m_Path,
				std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
			if (m_File.good()) {
				const std::vector<uint8_t> emptyHeader(HeaderSize, 0);
				m_File.write(reinterpret_cast<const char*>(emptyHeader.data()),
					static_cast<std::streamsize>(emptyHeader.size()));
				m_File.flush();
			}
		}

		m_Open = m_File.good();
		if (!m_Open) {
			AX_CORE_ERROR("RegionFile: failed to open {}", m_Path.string());
			return;
		}

		if (existed) {
			std::array<uint8_t, SectorSize> locationBytes{};
			std::array<uint8_t, SectorSize> timestampBytes{};
			m_File.seekg(0);
			m_File.read(reinterpret_cast<char*>(locationBytes.data()),
				static_cast<std::streamsize>(locationBytes.size()));
			m_File.read(reinterpret_cast<char*>(timestampBytes.data()),
				static_cast<std::streamsize>(timestampBytes.size()));
			if (!m_File.good()) {
				m_File.clear();
				AX_CORE_ERROR("RegionFile: failed to read header for {}", m_Path.string());
				m_Open = false;
				return;
			}
			for (int index = 0; index < ChunksPerRegion; ++index) {
				m_Offsets[index] = ReadBigEndianU32(locationBytes.data() + (index * 4));
				m_Timestamps[index] = ReadBigEndianU32(timestampBytes.data() + (index * 4));
			}
		}

		BuildSectorMap();
		AX_CORE_TRACE("RegionFile: opened {}", m_Path.filename().string());
	}

	void RegionFile::BuildSectorMap() {
		m_File.seekg(0, std::ios::end);
		const auto fileSize = m_File.tellg();
		const auto fileSizeBytes = fileSize > 0
			? static_cast<size_t>(fileSize)
			: static_cast<size_t>(HeaderSize);
		const uint32_t totalSectors = static_cast<uint32_t>(
			(fileSizeBytes + SectorSize - 1) / SectorSize);

		m_UsedSectors.assign(totalSectors, false);
		m_UsedSectors[0] = true;
		m_UsedSectors[1] = true;

		for (int index = 0; index < ChunksPerRegion; ++index) {
			const uint32_t location = m_Offsets[index];
			if (location == 0) {
				continue;
			}
			const uint32_t sectorOffset = (location >> 8) & 0xFFFFFFu;
			const uint32_t sectorCount = location & 0xFFu;
			for (uint32_t sector = 0; sector < sectorCount; ++sector) {
				const uint32_t position = sectorOffset + sector;
				if (position < m_UsedSectors.size()) {
					m_UsedSectors[position] = true;
				}
			}
		}
	}

	uint32_t RegionFile::AllocateSectors(uint32_t count) {
		const size_t total = m_UsedSectors.size();
		size_t runStart = 2;
		size_t runLength = 0;
		for (size_t position = 2; position < total; ++position) {
			if (m_UsedSectors[position]) {
				runStart = position + 1;
				runLength = 0;
				continue;
			}
			++runLength;
			if (runLength >= count) {
				for (size_t sector = runStart; sector < runStart + count; ++sector) {
					m_UsedSectors[sector] = true;
				}
				return static_cast<uint32_t>(runStart);
			}
		}

		const uint32_t newStart = static_cast<uint32_t>(total);
		m_UsedSectors.resize(total + count, true);
		return newStart;
	}

	void RegionFile::FreeSectors(uint32_t offset, uint32_t count) {
		for (uint32_t sector = 0; sector < count; ++sector) {
			const uint32_t position = offset + sector;
			if (position < m_UsedSectors.size()) {
				m_UsedSectors[position] = false;
			}
		}
	}

}
