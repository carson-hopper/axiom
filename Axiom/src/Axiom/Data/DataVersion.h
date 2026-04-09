#pragma once

#include <cstdint>
#include <string>

namespace Axiom {

	/**
	 * Data version metadata for world persistence.
	 * Tracks the Minecraft data version and provides
	 * migration hooks for future fixers.
	 */
	class DataVersion {
	public:
		static constexpr int32_t CURRENT_VERSION = 3953; // MC 1.21.4

		explicit DataVersion(int32_t version = CURRENT_VERSION);

		int32_t Version() const { return m_Version; }

		/**
		 * Whether this version is equal to or
		 * older than CURRENT_VERSION.
		 */
		bool IsCompatible() const;

		/**
		 * Whether this version is older than
		 * CURRENT_VERSION and needs
		 * data migration.
		 */
		bool NeedsMigration() const;

		/**
		 * Human-readable name for
		 * this data version.
		 */
		std::string VersionName() const;

	private:
		int32_t m_Version;
	};

}
