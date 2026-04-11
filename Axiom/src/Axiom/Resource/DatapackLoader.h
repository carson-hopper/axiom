#pragma once

#include "Axiom/Core/Base.h"

#include <filesystem>
#include <string>
#include <vector>

namespace Axiom {

	/**
	 * Represents a discovered datapack
	 * with its metadata and load path.
	 */
	struct DatapackInfo {
		std::string name;
		std::string description;
		int packFormat = 0;
		std::filesystem::path path;
		bool enabled = true;
	};

	/**
	 * Discovers and manages folder-based
	 * datapacks under the server's datapacks/
	 * directory. Zip datapacks are not yet
	 * supported.
	 */
	class DatapackLoader {
	public:
		DatapackLoader() = default;
		~DatapackLoader() = default;

		/**
		 * Scan the given directory for
		 * available datapacks.
		 */
		void Load(const std::filesystem::path& datapackDirectory);

		/**
		 * Rescan and reload all
		 * discovered datapacks.
		 */
		void Reload();

		const std::vector<DatapackInfo>& Datapacks() const { return m_Datapacks; }
		size_t Count() const { return m_Datapacks.size(); }

	private:
		void DiscoverDatapacks();

		std::filesystem::path m_DatapackDirectory;
		std::vector<DatapackInfo> m_Datapacks;
	};

}
