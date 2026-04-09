#pragma once

#include <filesystem>
#include <string>

#if defined(AX_PLATFORM_MACOS) || defined(AX_PLATFORM_LINUX)
	#include <mach-o/dyld.h>
	#include <limits.h>
#elif defined(AX_PLATFORM_WINDOWS)
	#include <windows.h>
#endif

namespace Axiom {

	/**
	 * Returns the directory containing the running executable.
	 */
	inline std::filesystem::path ExecutableDirectory() {
		#if defined(AX_PLATFORM_MACOS)
			char buffer[PATH_MAX];
			uint32_t size = sizeof(buffer);
			if (_NSGetExecutablePath(buffer, &size) == 0) {
				return std::filesystem::path(std::string(buffer)).parent_path();
			}
		#elif defined(AX_PLATFORM_LINUX)
			char buffer[PATH_MAX];
			ssize_t length = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
			if (length != -1) {
				buffer[length] = '\0';
				return std::filesystem::path(std::string(buffer, length)).parent_path();
			}
		#elif defined(AX_PLATFORM_WINDOWS)
			char buffer[MAX_PATH];
			GetModuleFileNameA(nullptr, buffer, MAX_PATH);
			return std::filesystem::path(std::string(buffer)).parent_path();
		#endif
		return std::filesystem::current_path();
	}

	/**
	 * Resolves a path relative to the executable directory.
	 * If the path doesn't exist there, falls back to current working directory.
	 */
	inline std::filesystem::path ResolvePath(const std::string& relativePath) {
		auto exeDir = ExecutableDirectory();
		auto candidate = exeDir / relativePath;
		if (std::filesystem::exists(candidate)) {
			return candidate;
		}
		// Try working directory
		return std::filesystem::current_path() / relativePath;
	}

}
