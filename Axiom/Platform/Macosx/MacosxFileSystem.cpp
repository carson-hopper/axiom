#include "axpch.h"
#include "Axiom/Utilities/FileSystem.h"

#include "Axiom/Core/Application.h"

#include <mach-o/dyld.h>

#include <climits>
#include <csignal>
#include <cerrno>
#include <filesystem>
#include <unistd.h>
#include <vector>

namespace Axiom {

	static std::filesystem::path s_PersistentStoragePath;

	FileStatus FileSystem::TryOpenFile(const std::filesystem::path& filepath) {
		const int res = access(filepath.c_str(), F_OK);

		if(!res) return FileStatus::Success;

		switch(errno) {
			default: return FileStatus::OtherError;

			case ENOENT: [[fallthrough]];
			case ENOTDIR: return FileStatus::Invalid;

			case EPERM: [[fallthrough]];
			case EACCES: return FileStatus::Locked;
		}
	}

	bool FileSystem::WriteBytes(const std::filesystem::path& filepath, const Buffer& buffer) {
		std::ofstream stream(filepath, std::ios::binary | std::ios::trunc);

		if (!stream) {
			stream.close();
			return false;
		}

		stream.write(static_cast<char*>(buffer.Data), buffer.Size);
		stream.close();

		return true;
	}

	Buffer FileSystem::ReadBytes(const std::filesystem::path& filepath) {
		Buffer buffer;

		std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

		const auto end = stream.tellg();
		stream.seekg(0, std::ios::beg);
		const auto size = end - stream.tellg();

		buffer.Allocate(static_cast<uint32_t>(size));
		stream.read(static_cast<char*>(buffer.Data), buffer.Size);
		stream.close();

		return buffer;
	}

	std::filesystem::path FileSystem::GetPersistentStoragePath() {
		if (!s_PersistentStoragePath.empty()) {
			return s_PersistentStoragePath;
		}

		if (HasEnvironmentVariable("AXIOM_DIR")) {
			s_PersistentStoragePath = GetEnvironmentVariable("AXIOM_DIR");
			s_PersistentStoragePath /= "Axiom";
		} else {
			uint32_t size = 0;
			_NSGetExecutablePath(nullptr, &size);
			std::vector<char> buffer(size);
			if (_NSGetExecutablePath(buffer.data(), &size) == 0) {
				// Executable lives at <targetdir>/Axiom. Its parent IS the
				// target dir already (where postbuild copies `data/`) — do
				// NOT append another "Axiom" segment.
				s_PersistentStoragePath = std::filesystem::path(std::string(buffer.data())).parent_path();
			} else {
				s_PersistentStoragePath = std::filesystem::current_path();
			}
		}

		if (!std::filesystem::exists(s_PersistentStoragePath)) {
			std::filesystem::create_directories(s_PersistentStoragePath);
		}

		return s_PersistentStoragePath;
	}

	bool FileSystem::HasEnvironmentVariable(const std::string& key) {
		return !GetEnvironmentVariable(key).empty();
	}

	bool FileSystem::SetEnvironmentVariable(const std::string& key, const std::string& value) {
		putenv((new std::string(std::format("{}={}", key, value)))->data());
		return true;
	}

	std::string FileSystem::GetEnvironmentVariable(const std::string& key) {
		const char* value = getenv(key.c_str());
		if (value)
			return std::string(value);
		return {};
	}

}