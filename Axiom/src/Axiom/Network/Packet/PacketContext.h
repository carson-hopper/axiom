#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Network/Crypto/ServerKeyPair.h"
#include "Axiom/Network/RegistryDataService.h"
#include "Axiom/Environment/Level/ChunkManager.h"
#include "Axiom/Environment/Entity/PlayerManager.h"
#include "Axiom/Network/KeepAliveManager.h"
#include "Axiom/Environment/Level/LevelTime.h"
#include "Axiom/Environment/Level/Physics/WorldTicker.h"
#include "Axiom/Environment/Level/Generator/ItemToBlockMapping.h"

#include <array>
#include <cstdint>
#include <mutex>
#include <random>
#include <string>
#include <unordered_map>

namespace Axiom {

	class Connection;
	class ServerConfig;
	class EventBus;
	class CommandRegistry;

	struct PendingLogin {
		std::string playerName;
		std::array<uint8_t, 4> verifyToken;
	};

	class PacketContext {
	public:
		PacketContext(ServerConfig& config, EventBus& eventBus, CommandRegistry& commands);
		~PacketContext() { m_WorldTicker.Stop(); m_WorldTime.Stop(); m_KeepAliveManager.Stop(); }

		ServerConfig& Config() { return m_Config; }
		EventBus& Events() { return m_EventBus; }
		CommandRegistry& Commands() { return m_Commands; }
		ServerKeyPair& KeyPair() { return m_KeyPair; }
		RegistryDataService& Registries() { return m_Registries; }
		ChunkManager& ChunkManagement() { return m_ChunkManager; }
		PlayerManager& Players() { return m_PlayerManager; }
		KeepAliveManager& KeepAlive() { return m_KeepAliveManager; }
		LevelTime& Time() { return m_WorldTime; }
		WorldTicker& Ticker() { return m_WorldTicker; }
		const ItemToBlockMapping& ItemBlocks() const { return m_ItemToBlock; }

		// ----- Pending logins -------------------------------------------

		void StorePendingLogin(ConnectionId connectionId, PendingLogin login);
		std::optional<PendingLogin> TakePendingLogin(ConnectionId connectionId);

		// ----- Utility --------------------------------------------------

		std::array<uint8_t, 4> GenerateVerifyToken();
		std::string FormatUuid(const std::string& trimmedUuid) const;
	private:
		ServerConfig& m_Config;
		EventBus& m_EventBus;
		CommandRegistry& m_Commands;
		ServerKeyPair m_KeyPair;
		RegistryDataService m_Registries;
		ChunkManager m_ChunkManager;
		PlayerManager m_PlayerManager;
		KeepAliveManager m_KeepAliveManager{m_PlayerManager};
		ItemToBlockMapping m_ItemToBlock;
		LevelTime m_WorldTime{m_PlayerManager};
		WorldTicker m_WorldTicker{m_PlayerManager};

		std::mutex m_PendingLoginsMutex;
		std::unordered_map<ConnectionId, PendingLogin> m_PendingLogins;

		std::mt19937 m_Random{std::random_device{}()};
	};

}
