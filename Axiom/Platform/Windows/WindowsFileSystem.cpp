#include "axpch.h"
#include "Axiom/Utilities/FileSystem.h"

#include "Axiom//Core/Application.h"

#include <Windows.h>

#include <filesystem>

namespace Axiom {

	static std::filesystem::path s_PersistentStoragePath;

	FileStatus FileSystem::TryOpenFile(const std::filesystem::path& filepath) {
		HANDLE fileHandle = CreateFile(filepath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr);
		if (fileHandle == INVALID_HANDLE_VALUE) {
			DWORD errorCode = GetLastError();
			if (errorCode == ERROR_FILE_NOT_FOUND || errorCode == ERROR_PATH_NOT_FOUND)
				return FileStatus::Invalid;
			if (errorCode == ERROR_SHARING_VIOLATION)
				return FileStatus::Locked;

			return FileStatus::OtherError;
		}

		CloseHandle(fileHandle);
		return FileStatus::Success;
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
		AX_CORE_ASSERT(stream);

		const auto end = stream.tellg();
		stream.seekg(0, std::ios::beg);
		const auto size = end - stream.tellg();
		AX_CORE_ASSERT(size != 0);

		buffer.Allocate(static_cast<uint32_t>(size));
		stream.read(static_cast<char*>(buffer.Data), buffer.Size);
		stream.close();

		return buffer;
	}

	std::filesystem::path FileSystem::GetPersistentStoragePath() {
		if (!s_PersistentStoragePath.empty())
			return s_PersistentStoragePath;

		PWSTR roamingFilePath;
		HRESULT result = SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_DEFAULT, NULL, &roamingFilePath);
		HZ_CORE_VERIFY(result == S_OK);
		s_PersistentStoragePath = roamingFilePath;

		if (!std::filesystem::exists(s_PersistentStoragePath))
			std::filesystem::create_directory(s_PersistentStoragePath);

		return s_PersistentStoragePath;
	}

	bool FileSystem::HasEnvironmentVariable(const std::string& key) {
		HKEY hKey;
		LSTATUS lOpenStatus = RegOpenKeyExA(HKEY_CURRENT_USER, "Environment", 0, KEY_ALL_ACCESS, &hKey);

		if (lOpenStatus == ERROR_SUCCESS) {
			lOpenStatus = RegQueryValueExA(hKey, key.c_str(), 0, NULL, NULL, NULL);
			RegCloseKey(hKey);
		}

		return lOpenStatus == ERROR_SUCCESS;
	}

	bool FileSystem::SetEnvironmentVariable(const std::string& key, const std::string& value) {
		HKEY hKey;
		LPCSTR keyPath = "Environment";
		DWORD createdNewKey;
		LSTATUS lOpenStatus = RegCreateKeyExA(HKEY_CURRENT_USER, keyPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &createdNewKey);
		if (lOpenStatus == ERROR_SUCCESS) {
			LSTATUS lSetStatus = RegSetValueExA(hKey, key.c_str(), 0, REG_SZ, static_cast<LPBYTE>(value.c_str()), static_cast<DWORD>(value.length() + 1));
			RegCloseKey(hKey);

			if (lSetStatus == ERROR_SUCCESS) {
				SendMessageTimeoutA(HWND_BROADCAST, WM_SETTINGCHANGE, 0, static_cast<LPARAM>("Environment"), SMTO_BLOCK, 100, NULL);
				return true;
			}
		}

		return false;
	}

	std::string FileSystem::GetEnvironmentVariable(const std::string& key) {
		const char* value = getenv(key.c_str());
		if (value)
			return std::string(value);
		return {};
	}


}