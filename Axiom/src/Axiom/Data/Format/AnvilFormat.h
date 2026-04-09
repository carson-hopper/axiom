#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Data/WorldFormat.h"
#include "Axiom/Data/Format/RegionFileStorage.h"

#include <filesystem>

namespace Axiom {

	/**
	 * Anvil world format implementation.
	 * Uses .mca region files with 4KiB
	 * sector allocation, matching the
	 * vanilla Minecraft format.
	 */
	class AnvilFormat : public WorldFormat {
	public:
		explicit AnvilFormat(std::filesystem::path worldDirectory);
		~AnvilFormat() override = default;

		std::optional<std::vector<uint8_t>> LoadChunk(int32_t chunkX, int32_t chunkZ) override;
		void SaveChunk(int32_t chunkX, int32_t chunkZ, const std::vector<uint8_t>& data) override;

		std::optional<std::vector<uint8_t>> LoadLevelData() override;
		void SaveLevelData(const std::vector<uint8_t>& data) override;

		std::optional<std::vector<uint8_t>> LoadPlayerData(const std::string& uuid) override;
		void SavePlayerData(const std::string& uuid, const std::vector<uint8_t>& data) override;

		void Flush() override;

		std::string GetFormatName() const override { return "Anvil"; }
		bool SupportsAsyncIO() const override { return true; }

	private:
		std::filesystem::path m_WorldDirectory;
		std::filesystem::path m_PlayerDataDirectory;
		RegionFileStorage m_RegionStorage;
	};

}
