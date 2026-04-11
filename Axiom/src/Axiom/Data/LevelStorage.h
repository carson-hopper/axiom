#pragma once

#include <string>
#include <unordered_map>

namespace Axiom {

	class LevelStorage {
	public:
		explicit LevelStorage(const std::string& worldDirectory);

		bool AcquireSessionLock();
		void ReleaseSessionLock();

		std::unordered_map<std::string, std::string> LoadLevelData();
		void SaveLevelData(const std::unordered_map<std::string, std::string>& data);

		void EnsureDirectories();

		const std::string& WorldDirectory() const { return m_WorldDirectory; }

	private:
		std::string m_WorldDirectory;
		bool m_HasSessionLock = false;
	};

}
