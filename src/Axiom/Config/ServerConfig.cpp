#include "ServerConfig.h"

#include "Axiom/Core/Log.h"

#include <fstream>

namespace Axiom {

	void ServerConfig::Load(const std::string& path) {
		m_Path = path;

		try {
			std::ifstream file(path);
			if (file.good()) {
				m_Root = YAML::LoadFile(path);
				AX_CORE_INFO("Loaded configuration from {}", path);
			} else {
				AX_CORE_INFO("No configuration file found, creating defaults");
				SetDefaults();
				Save();
			}
		} catch (const YAML::Exception& exception) {
			AX_CORE_ERROR("Failed to load configuration: {}", exception.what());
			SetDefaults();
		}
	}

	void ServerConfig::Save() {
		try {
			std::ofstream file(m_Path);
			file << m_Root;
			AX_CORE_INFO("Saved configuration to {}", m_Path);
		} catch (const std::exception& exception) {
			AX_CORE_ERROR("Failed to save configuration: {}", exception.what());
		}
	}

	std::string ServerConfig::ServerName() const {
		return m_Root["server-name"].as<std::string>("Axiom Server");
	}

	uint16_t ServerConfig::Port() const {
		return m_Root["port"].as<uint16_t>(25565);
	}

	int ServerConfig::MaxPlayers() const {
		return m_Root["max-players"].as<int>(20);
	}

	int ServerConfig::ViewDistance() const {
		return m_Root["view-distance"].as<int>(10);
	}

	std::string ServerConfig::Motd() const {
		return m_Root["motd"].as<std::string>("An Axiom Server");
	}

	bool ServerConfig::OnlineMode() const {
		return m_Root["online-mode"].as<bool>(true);
	}

	int ServerConfig::TickRate() const {
		return m_Root["tick-rate"].as<int>(20);
	}

	void ServerConfig::SetDefaults() {
		m_Root["server-name"] = "Axiom Server";
		m_Root["port"] = 25565;
		m_Root["max-players"] = 20;
		m_Root["view-distance"] = 10;
		m_Root["motd"] = "An Axiom Server";
		m_Root["online-mode"] = true;
		m_Root["tick-rate"] = 20;
	}

}
