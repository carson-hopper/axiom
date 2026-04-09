#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Core/Math.h"

#include <cstdint>
#include <string>
#include <vector>

namespace Axiom {

	class Level;

	/**
	 * Abstract base for tile entities attached
	 * to specific block positions. Block entities
	 * store extra data beyond block state (chest
	 * contents, sign text, furnace progress, etc.).
	 */
	class BlockEntity {
	public:
		BlockEntity(std::string type, int32_t positionX, int32_t positionY, int32_t positionZ)
			: m_Type(std::move(type))
			, m_PositionX(positionX)
			, m_PositionY(positionY)
			, m_PositionZ(positionZ) {}

		virtual ~BlockEntity() = default;

		// ----- Position -------------------------------------------------

		int32_t PositionX() const { return m_PositionX; }
		int32_t PositionY() const { return m_PositionY; }
		int32_t PositionZ() const { return m_PositionZ; }
		const std::string& Type() const { return m_Type; }

		// ----- Level reference ------------------------------------------

		Level* GetLevel() const { return m_Level; }
		void SetLevel(Level* level) { m_Level = level; }

		// ----- Persistence hooks ----------------------------------------

		/**
		 * Save additional data beyond
		 * position and type. Subclasses
		 * append their fields here.
		 */
		virtual void SaveAdditional(std::vector<uint8_t>& data) const {
			(void)data;
		}

		/**
		 * Load additional data saved by
		 * SaveAdditional. Subclasses
		 * read their fields here.
		 */
		virtual void LoadAdditional(const std::vector<uint8_t>& data) {
			(void)data;
		}

		// ----- Dirty flag -----------------------------------------------

		/**
		 * Mark this block entity as needing
		 * to be saved on the next flush.
		 */
		void SetChanged() { m_Changed = true; }

		/**
		 * Whether this block entity has
		 * unsaved modifications.
		 */
		bool IsChanged() const { return m_Changed; }

		/**
		 * Clear the dirty flag after
		 * saving to disk.
		 */
		void ClearChanged() { m_Changed = false; }

		// ----- Network sync ---------------------------------------------

		/**
		 * Get the data tag sent to clients
		 * for initial chunk data or block
		 * entity updates.
		 */
		virtual std::vector<uint8_t> GetUpdateTag() const {
			return {};
		}

		// ----- Lifecycle ------------------------------------------------

		/**
		 * Mark this block entity as removed.
		 * Called when the block is broken
		 * or replaced.
		 */
		void SetRemoved() { m_Removed = true; }

		/**
		 * Clear the removed flag, used when
		 * a block entity is re-placed or
		 * during undo operations.
		 */
		void ClearRemoved() { m_Removed = false; }

		/**
		 * Whether this block entity has
		 * been removed from the world.
		 */
		bool IsRemoved() const { return m_Removed; }

		/**
		 * Called each tick for block entities
		 * that need per-tick updates (furnaces,
		 * hoppers, etc.).
		 */
		virtual void Tick() {}

	private:
		std::string m_Type;
		int32_t m_PositionX;
		int32_t m_PositionY;
		int32_t m_PositionZ;
		Level* m_Level = nullptr;
		bool m_Changed = false;
		bool m_Removed = false;
	};

}
