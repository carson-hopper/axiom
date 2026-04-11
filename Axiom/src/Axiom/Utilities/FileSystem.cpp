#include "axpch.h"
#include "Axiom/Utilities/FileSystem.h"

namespace Axiom {

	std::filesystem::path FileSystem::GetWorkingDirectory() {
		return std::filesystem::current_path();
	}

	void FileSystem::SetWorkingDirectory(const std::filesystem::path &path) {
		std::filesystem::current_path(path);
	}

	bool FileSystem::CreateDirectory(const std::filesystem::path& directory) {
		return std::filesystem::create_directories(directory);
	}

	bool FileSystem::CreateDirectory(const std::string& directory) {
		return CreateDirectory(std::filesystem::path(directory));
	}

	bool FileSystem::Move(const std::filesystem::path& oldFilepath, const std::filesystem::path& newFilepath) {
		if (Exists(newFilepath))
			return false;

		std::filesystem::rename(oldFilepath, newFilepath);
		return true;
	}

	bool FileSystem::Copy(const std::filesystem::path& oldFilepath, const std::filesystem::path& newFilepath) {
		if (Exists(newFilepath))
			return false;

		std::filesystem::copy(oldFilepath, newFilepath);
		return true;
	}

	bool FileSystem::MoveFile(const std::filesystem::path& filepath, const std::filesystem::path& dest) {
		return Move(filepath, dest / filepath.filename());
	}

	bool FileSystem::CopyFile(const std::filesystem::path& filepath, const std::filesystem::path& dest) {
		return Copy(filepath, dest / filepath.filename());
	}

	bool FileSystem::Rename(const std::filesystem::path& oldFilepath, const std::filesystem::path& newFilepath) {
		return Move(oldFilepath, newFilepath);
	}

	bool FileSystem::RenameFilename(const std::filesystem::path& oldFilepath, const std::string& newName) {
		const std::filesystem::path newPath = oldFilepath.parent_path() / std::filesystem::path(newName + oldFilepath.extension().string());
		return Rename(oldFilepath, newPath);
	}

	bool FileSystem::Exists(const std::filesystem::path& filepath) {
		return std::filesystem::exists(filepath);
	}

	bool FileSystem::Exists(const std::string& filepath) {
		return std::filesystem::exists(std::filesystem::path(filepath));
	}

	bool FileSystem::DeleteFile(const std::filesystem::path& filepath) {
		if (!Exists(filepath))
			return false;

		if (std::filesystem::is_directory(filepath))
			return std::filesystem::remove_all(filepath) > 0;
		return std::filesystem::remove(filepath);
	}

	bool FileSystem::IsDirectory(const std::filesystem::path& filepath) {
		return std::filesystem::is_directory(filepath);
	}

	FileStatus FileSystem::TryOpenFileAndWait(const std::filesystem::path& filepath, const uint64_t waitms) {
		const FileStatus fileStatus = TryOpenFile(filepath);
		if (fileStatus == FileStatus::Locked) {
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(operator""ms(waitms));
			return TryOpenFile(filepath);
		}
		return fileStatus;
	}

	// returns true <=> fileA was last modified more recently than fileB
	bool FileSystem::IsNewer(const std::filesystem::path& fileA, const std::filesystem::path& fileB) {
		return std::filesystem::last_write_time(fileA) > std::filesystem::last_write_time(fileB);
	}

	uint64_t FileSystem::GetLastWriteTime(const std::filesystem::path& filepath) {
		if (TryOpenFileAndWait(filepath) == FileStatus::Success) {
			std::filesystem::file_time_type lastWriteTime = std::filesystem::last_write_time(filepath);
			return std::chrono::duration_cast<std::chrono::seconds>(lastWriteTime.time_since_epoch()).count();
		}

		AX_CORE_ERROR("FileSystem::GetLastWriteTime - could not open file: {}", filepath.string());
		return 0;
	}

}