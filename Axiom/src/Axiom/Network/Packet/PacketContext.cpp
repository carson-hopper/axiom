#include "PacketContext.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Core/PathUtil.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Protocol.h"
#include "Axiom/Config/ServerConfig.h"
#include "Axiom/Environment/Level/Generator/VanillaChunkGenerator.h"

namespace Axiom {

	static constexpr int32_t COMPRESSION_THRESHOLD = 256;

	PacketContext::PacketContext(ServerConfig& config, EventBus& eventBus,
		CommandRegistry& commands, NetworkServer& server)
		: m_Config(config)
		, m_EventBus(eventBus)
		, m_Commands(commands)
		, m_Server(server)
		, m_ChunkManager(CreateRef<VanillaChunkGenerator>("./worlds/minecraft/overworld"), config.ViewDistance()) {

		const auto dataPath = ResolvePath("data");
		m_Registries.LoadAll(dataPath.string());
		m_ItemToBlock.LoadFromExtractorData(dataPath.string());
		m_KeepAliveManager.Start();
		m_WorldTime.Start();
		m_WorldTicker.SetTerrainLookup(
			[this](const int32_t worldX, const int32_t worldY, const int32_t worldZ) -> int32_t {
				return m_ChunkManager.Generator().GetBlockAt(worldX, worldY, worldZ);
			});
		m_ChunkManager.SetChunkSentCallback(
			[this](const ChunkPosition chunkPosition) {
				m_WorldTicker.ScanChunkForPhysics(chunkPosition.x, chunkPosition.z);
			});

		// Save dirty chunks back to region files on unload
		m_ChunkManager.SetChunkUnloadCallback(
			[this](const ChunkPosition chunkPosition) {
				const auto& generator = dynamic_cast<VanillaChunkGenerator&>(m_ChunkManager.Generator());
				generator.SaveChunk(chunkPosition.x, chunkPosition.z, m_WorldTicker);
			});
		// Mark chunks dirty when blocks are modified
		m_WorldTicker.SetBlockDirtyCallback(
			[this](const int32_t chunkX, const int32_t chunkZ) {
				m_ChunkManager.MarkChunkDirty(chunkX, chunkZ);
			});

		m_WorldTicker.Start();
	}

	void PacketContext::StorePendingLogin(const ConnectionId connectionId, PendingLogin login) {
		std::scoped_lock const lock(m_PendingLoginsMutex);
		m_PendingLogins[connectionId] = std::move(login);
	}

	std::optional<PendingLogin> PacketContext::TakePendingLogin(const ConnectionId connectionId) {
		std::scoped_lock const lock(m_PendingLoginsMutex);
		const auto iterator = m_PendingLogins.find(connectionId);
		if (iterator == m_PendingLogins.end()) {
			return std::nullopt;
		}
		auto login = std::move(iterator->second);
		m_PendingLogins.erase(iterator);
		return login;
	}

	std::array<uint8_t, 4> PacketContext::GenerateVerifyToken() {
		std::array<uint8_t, 4> token{};
		std::uniform_int_distribution<int> distribution(0, 255);
		for (auto& byte : token) {
			byte = static_cast<uint8_t>(distribution(m_Random));
		}
		return token;
	}

	std::string PacketContext::FormatUuid(const std::string& trimmedUuid) const {
		if (trimmedUuid.size() != 32) {
			return trimmedUuid;
		}
		return trimmedUuid.substr(0, 8) + "-"
			+ trimmedUuid.substr(8, 4) + "-"
			+ trimmedUuid.substr(12, 4) + "-"
			+ trimmedUuid.substr(16, 4) + "-"
			+ trimmedUuid.substr(20, 12);
	}

}
