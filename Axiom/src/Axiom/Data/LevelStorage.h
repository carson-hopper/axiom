#pragma once

#include <string>
#include <unordered_map>

namespace Axiom {

	/**
	 * Handles level.dat, level.dat_old, and session.lock
	 * for a single world directory. Provides directory
	 * scaffolding and placeholder key-value persistence
	 * until full NBT serialization in Task 12.
	 */
	class LevelStorage {
	public:
		explicit LevelStorage(const std::string& worldDirectory);

		/**
		 * Acquire session.lock by writing the current
		 * timestamp. Returns false if already locked.
		 */
		bool AcquireSessionLock();

		/**
		 * Release the session lock and
		 * remove the lock file.
		 */
		void ReleaseSessionLock();

		/**
		 * Load level.dat (currently a JSON placeholder).
		 * Returns empty map on failure or if
		 * the file does not exist.
		 */
		std::unordered_map<std::string, std::string> LoadLevelData();

		/**
		 * Save level.dat, backing up the previous file
		 * to level.dat_old first. Currently writes
		 * a JSON placeholder format.
		 */
		void SaveLevelData(const std::unordered_map<std::string, std::string>& data);

		/**
		 * Create the full world directory structure
		 * if any subdirectories are missing.
		 */
		void EnsureDirectories();

		/**
		 * Get the world directory
		 * path.
		 */
		const std::string& WorldDirectory() const { return m_WorldDirectory; }

	private:
		std::string m_WorldDirectory;
		bool m_HasSessionLock = false;
	};

}
