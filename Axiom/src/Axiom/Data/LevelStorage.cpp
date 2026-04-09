#include "LevelStorage.h"

#include "Axiom/Core/Log.h"

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>

#include <nlohmann/json.hpp>

namespace Axiom {

LevelStorage::LevelStorage(const std::string& worldDirectory)
	: m_WorldDirectory(worldDirectory) {}

bool LevelStorage::AcquireSessionLock() {
	if (m_HasSessionLock) {
		return true;
	}

	EnsureDirectories();

	const std::string lockPath = m_WorldDirectory + "/session.lock";

	/**
	 * Check whether another process already
	 * holds the session lock file.
	 */
	if (std::filesystem::exists(lockPath)) {
		AX_CORE_WARN("session.lock already exists at {}", lockPath);
		return false;
	}

	/**
	 * Write the current timestamp as 8 bytes
	 * in big-endian order, matching the
	 * vanilla Minecraft format.
	 */
	std::ofstream lockFile(lockPath, std::ios::binary);
	if (!lockFile) {
		AX_CORE_ERROR("Failed to create session.lock at {}", lockPath);
		return false;
	}

	const auto now = std::chrono::system_clock::now();
	const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(
		now.time_since_epoch()
	).count();
	const int64_t timestamp = static_cast<int64_t>(millis);

	for (int shift = 56; shift >= 0; shift -= 8) {
		const uint8_t byte = static_cast<uint8_t>((timestamp >> shift) & 0xFF);
		lockFile.write(reinterpret_cast<const char*>(&byte), 1);
	}

	lockFile.close();
	m_HasSessionLock = true;
	AX_CORE_INFO("Acquired session.lock for {}", m_WorldDirectory);
	return true;
}

void LevelStorage::ReleaseSessionLock() {
	if (!m_HasSessionLock) {
		return;
	}

	const std::string lockPath = m_WorldDirectory + "/session.lock";
	std::filesystem::remove(lockPath);
	m_HasSessionLock = false;
	AX_CORE_INFO("Released session.lock for {}", m_WorldDirectory);
}

std::unordered_map<std::string, std::string> LevelStorage::LoadLevelData() {
	std::unordered_map<std::string, std::string> result;
	const std::string levelPath = m_WorldDirectory + "/level.dat";

	std::ifstream file(levelPath);
	if (!file) {
		AX_CORE_WARN("No level.dat found at {}", levelPath);
		return result;
	}

	/**
	 * Parse the JSON placeholder format.
	 * Full NBT persistence will replace
	 * this in Task 12.
	 */
	try {
		nlohmann::json document = nlohmann::json::parse(file);
		for (auto& [key, value] : document.items()) {
			if (value.is_string()) {
				result[key] = value.get<std::string>();
			} else {
				result[key] = value.dump();
			}
		}
		AX_CORE_INFO("Loaded level.dat from {}", levelPath);
	} catch (const nlohmann::json::exception& exception) {
		AX_CORE_ERROR("Failed to parse level.dat: {}", exception.what());
	}

	return result;
}

void LevelStorage::SaveLevelData(
	const std::unordered_map<std::string, std::string>& data
) {
	EnsureDirectories();

	const std::string levelPath = m_WorldDirectory + "/level.dat";
	const std::string backupPath = m_WorldDirectory + "/level.dat_old";

	/**
	 * Back up the existing level.dat to
	 * level.dat_old before overwriting.
	 */
	if (std::filesystem::exists(levelPath)) {
		std::error_code errorCode;
		std::filesystem::copy_file(
			levelPath, backupPath,
			std::filesystem::copy_options::overwrite_existing,
			errorCode
		);
		if (errorCode) {
			AX_CORE_WARN(
				"Failed to back up level.dat: {}",
				errorCode.message()
			);
		}
	}

	/**
	 * Write the key-value data as JSON.
	 * Full NBT persistence will replace
	 * this in Task 12.
	 */
	nlohmann::json document;
	for (const auto& [key, value] : data) {
		document[key] = value;
	}

	std::ofstream file(levelPath);
	if (!file) {
		AX_CORE_ERROR("Failed to write level.dat at {}", levelPath);
		return;
	}

	file << document.dump(4);
	AX_CORE_INFO("Saved level.dat to {}", levelPath);
}

void LevelStorage::EnsureDirectories() {
	const std::string subdirectories[] = {
		"region",
		"entities",
		"poi",
		"playerdata",
		"datapacks",
		"advancements",
		"stats",
		"data"
	};

	std::filesystem::create_directories(m_WorldDirectory);

	for (const auto& subdirectory : subdirectories) {
		const std::string fullPath = m_WorldDirectory + "/" + subdirectory;
		std::filesystem::create_directories(fullPath);
	}
}

}
