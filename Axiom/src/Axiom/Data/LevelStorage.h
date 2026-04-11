#pragma once

#include <string>
#include <unordered_map>

namespace Axiom {

	/**
	 * Handles level.dat, level.dat_old, and session.lock
	 * for a single world directory. Provides directory
	 * scaffolding and flat key-value persistence backed
	 * by gzip-compressed NBT — the same on-disk format
	 * vanilla Minecraft uses.
	 *
	 * Callers that want typed fields (ints, longs, lists,
	 * nested compounds) should graduate to the `NbtIo` +
	 * `NbtCompound` API directly; this class exists for
	 * the simple string-map path that boots the server
	 * against a valid world layout.
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
		 * Load level.dat as a gzip-compressed NBT
		 * compound, returning every child tag as a
		 * string-keyed entry. String tags pass through
		 * verbatim; other tag types are rendered via
		 * `ToString()` so callers can still inspect them
		 * by key. Returns an empty map on failure or if
		 * the file does not exist.
		 */
		std::unordered_map<std::string, std::string> LoadLevelData();

		/**
		 * Save level.dat as a gzip-compressed NBT
		 * compound. Every entry in `data` becomes a
		 * `TAG_String` child. The previous file is
		 * backed up to level.dat_old before the new
		 * file is written.
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
