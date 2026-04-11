#include "LevelStorage.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Data/Nbt/NbtCompound.h"
#include "Axiom/Data/Nbt/NbtIo.h"
#include "Axiom/Data/Nbt/NbtListImpl.h"
#include "Axiom/Data/Nbt/NbtString.h"

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iterator>

namespace Axiom {

LevelStorage::LevelStorage(const std::string& worldDirectory)
	: m_WorldDirectory(worldDirectory) {}

bool LevelStorage::AcquireSessionLock() {
	if (m_HasSessionLock) {
		return true;
	}

	EnsureDirectories();

	const std::string lockPath = m_WorldDirectory + "/session.lock";

	if (std::filesystem::exists(lockPath)) {
		AX_CORE_WARN("session.lock already exists at {}", lockPath);
		return false;
	}

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

	if (!std::filesystem::exists(levelPath)) {
		AX_CORE_WARN("No level.dat found at {}", levelPath);
		return result;
	}

	std::ifstream file(levelPath, std::ios::binary);
	if (!file) {
		AX_CORE_WARN("Failed to open level.dat at {}", levelPath);
		return result;
	}

	const std::vector<uint8_t> bytes(
		(std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());

	try {
		const auto root = NbtIo::ReadGzipCompressed(bytes);
		if (!root) {
			AX_CORE_WARN("level.dat at {} is empty or unreadable", levelPath);
			return result;
		}

		for (const auto& [name, tag] : root->Tags()) {
			if (!tag) {
				continue;
			}
			if (tag->Type() == NbtTagType::String) {
				result[name] = static_cast<NbtString*>(tag.Raw())->Value();
			} else {
				result[name] = tag->ToString();
			}
		}
		AX_CORE_INFO("Loaded level.dat from {} ({} tags)", levelPath, result.size());
	} catch (const std::exception& exception) {
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

	auto root = Ref<NbtCompound>::Create();
	for (const auto& [key, value] : data) {
		root->PutString(key, value);
	}

	std::vector<uint8_t> bytes;
	try {
		bytes = NbtIo::WriteGzipCompressed(root, "");
	} catch (const std::exception& exception) {
		AX_CORE_ERROR("Failed to serialize level.dat: {}", exception.what());
		return;
	}

	std::ofstream file(levelPath, std::ios::binary | std::ios::trunc);
	if (!file) {
		AX_CORE_ERROR("Failed to write level.dat at {}", levelPath);
		return;
	}

	file.write(
		reinterpret_cast<const char*>(bytes.data()),
		static_cast<std::streamsize>(bytes.size()));

	AX_CORE_INFO("Saved level.dat to {} ({} bytes)", levelPath, bytes.size());
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
