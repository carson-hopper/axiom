#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace Axiom {

	/**
	 * Abstract interface for world persistence formats.
	 * Implementations handle chunk, level, and player
	 * data serialization to a specific storage
	 * backend (Anvil region files, etc.).
	 */
	class WorldFormat {
	public:
		virtual ~WorldFormat() = default;

		/**
		 * Load raw chunk data from storage.
		 * Returns empty if the chunk has never
		 * been saved.
		 */
		virtual std::optional<std::vector<uint8_t>> LoadChunk(int32_t chunkX, int32_t chunkZ) = 0;

		/**
		 * Persist raw chunk data to storage.
		 * The data vector contains the
		 * serialized chunk bytes.
		 */
		virtual void SaveChunk(int32_t chunkX, int32_t chunkZ, const std::vector<uint8_t>& data) = 0;

		/**
		 * Load the level.dat contents.
		 * Returns empty if no level
		 * data exists yet.
		 */
		virtual std::optional<std::vector<uint8_t>> LoadLevelData() = 0;

		/**
		 * Save the level.dat contents
		 * to persistent storage.
		 */
		virtual void SaveLevelData(const std::vector<uint8_t>& data) = 0;

		/**
		 * Load saved data for a specific player.
		 * Returns empty if the player has
		 * no saved data yet.
		 */
		virtual std::optional<std::vector<uint8_t>> LoadPlayerData(const std::string& uuid) = 0;

		/**
		 * Save player data to persistent
		 * storage keyed by UUID.
		 */
		virtual void SavePlayerData(const std::string& uuid, const std::vector<uint8_t>& data) = 0;

		/**
		 * Flush any buffered writes to disk.
		 * Called during save intervals and
		 * on server shutdown.
		 */
		virtual void Flush() = 0;

		/**
		 * Get a human-readable name for
		 * this storage format.
		 */
		virtual std::string GetFormatName() const = 0;

		/**
		 * Whether this format supports
		 * asynchronous I/O operations.
		 */
		virtual bool SupportsAsyncIO() const = 0;
	};

}
