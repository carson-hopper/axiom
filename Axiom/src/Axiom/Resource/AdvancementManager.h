#pragma once

#include "Axiom/Core/Base.h"

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace Axiom {

	/**
	 * Represents a loaded advancement with
	 * its identifier and parent chain.
	 */
	struct AdvancementEntry {
		std::string name;
		std::string parent;
		std::filesystem::path sourcePath;
	};

	/**
	 * Loads advancement JSON definitions
	 * that describe the achievement tree.
	 * Advancements form a directed acyclic
	 * graph via parent references.
	 */
	class AdvancementManager {
	public:
		AdvancementManager() = default;
		~AdvancementManager() = default;

		/**
		 * Load all advancements from
		 * the given directory.
		 */
		void Load(const std::filesystem::path& advancementDirectory);

		/**
		 * Reload all advancements from
		 * the last loaded directory.
		 */
		void Reload();

		const std::vector<AdvancementEntry>& Advancements() const { return m_Advancements; }
		size_t Count() const { return m_Advancements.size(); }

	private:
		void LoadDirectory(const std::filesystem::path& directory);

		std::filesystem::path m_AdvancementDirectory;
		std::vector<AdvancementEntry> m_Advancements;
	};

}
