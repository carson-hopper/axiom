#include "AnvilFormat.h"

#include "Axiom/Core/Log.h"

#include <fstream>

namespace Axiom {

	AnvilFormat::AnvilFormat(std::filesystem::path worldDirectory)
		: m_WorldDirectory(std::move(worldDirectory))
		, m_PlayerDataDirectory(m_WorldDirectory / "playerdata")
		, m_RegionStorage(m_WorldDirectory / "region") {
		if (!std::filesystem::exists(m_PlayerDataDirectory)) {
			std::filesystem::create_directories(m_PlayerDataDirectory);
		}
		AX_CORE_INFO("AnvilFormat: initialized for world at {}", m_WorldDirectory.string());
	}

	std::optional<std::vector<uint8_t>> AnvilFormat::LoadChunk(int32_t chunkX, int32_t chunkZ) {
		AX_CORE_TRACE("AnvilFormat: loading chunk ({}, {})", chunkX, chunkZ);

		auto regionFile = m_RegionStorage.GetRegionFile(chunkX, chunkZ);
		int localX = RegionFile::ChunkToLocal(chunkX);
		int localZ = RegionFile::ChunkToLocal(chunkZ);

		return regionFile->ReadChunk(localX, localZ);
	}

	void AnvilFormat::SaveChunk(int32_t chunkX, int32_t chunkZ, const std::vector<uint8_t>& data) {
		AX_CORE_TRACE("AnvilFormat: saving chunk ({}, {}) ({} bytes)", chunkX, chunkZ, data.size());

		auto regionFile = m_RegionStorage.GetRegionFile(chunkX, chunkZ);
		int localX = RegionFile::ChunkToLocal(chunkX);
		int localZ = RegionFile::ChunkToLocal(chunkZ);

		regionFile->WriteChunk(localX, localZ, data);
	}

	std::optional<std::vector<uint8_t>> AnvilFormat::LoadLevelData() {
		auto levelPath = m_WorldDirectory / "level.dat";

		if (!std::filesystem::exists(levelPath)) {
			AX_CORE_TRACE("AnvilFormat: no level.dat found at {}", levelPath.string());
			return std::nullopt;
		}

		AX_CORE_TRACE("AnvilFormat: loading level.dat");

		std::ifstream file(levelPath, std::ios::binary);
		if (!file.good()) {
			AX_CORE_ERROR("AnvilFormat: failed to open level.dat");
			return std::nullopt;
		}

		std::vector<uint8_t> data(
			(std::istreambuf_iterator<char>(file)),
			std::istreambuf_iterator<char>());

		return data;
	}

	void AnvilFormat::SaveLevelData(const std::vector<uint8_t>& data) {
		auto levelPath = m_WorldDirectory / "level.dat";

		AX_CORE_TRACE("AnvilFormat: saving level.dat ({} bytes)", data.size());

		std::ofstream file(levelPath, std::ios::binary | std::ios::trunc);
		if (!file.good()) {
			AX_CORE_ERROR("AnvilFormat: failed to write level.dat");
			return;
		}

		file.write(reinterpret_cast<const char*>(data.data()),
			static_cast<std::streamsize>(data.size()));
	}

	std::optional<std::vector<uint8_t>> AnvilFormat::LoadPlayerData(const std::string& uuid) {
		auto playerPath = m_PlayerDataDirectory / (uuid + ".dat");

		if (!std::filesystem::exists(playerPath)) {
			AX_CORE_TRACE("AnvilFormat: no player data for {}", uuid);
			return std::nullopt;
		}

		AX_CORE_TRACE("AnvilFormat: loading player data for {}", uuid);

		std::ifstream file(playerPath, std::ios::binary);
		if (!file.good()) {
			AX_CORE_ERROR("AnvilFormat: failed to read player data for {}", uuid);
			return std::nullopt;
		}

		std::vector<uint8_t> data(
			(std::istreambuf_iterator<char>(file)),
			std::istreambuf_iterator<char>());

		return data;
	}

	void AnvilFormat::SavePlayerData(const std::string& uuid, const std::vector<uint8_t>& data) {
		auto playerPath = m_PlayerDataDirectory / (uuid + ".dat");

		AX_CORE_TRACE("AnvilFormat: saving player data for {} ({} bytes)", uuid, data.size());

		std::ofstream file(playerPath, std::ios::binary | std::ios::trunc);
		if (!file.good()) {
			AX_CORE_ERROR("AnvilFormat: failed to write player data for {}", uuid);
			return;
		}

		file.write(reinterpret_cast<const char*>(data.data()),
			static_cast<std::streamsize>(data.size()));
	}

	void AnvilFormat::Flush() {
		AX_CORE_INFO("AnvilFormat: flushing all region files");
		m_RegionStorage.FlushAll();
	}

}
