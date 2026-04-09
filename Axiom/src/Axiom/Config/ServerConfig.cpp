#include "ServerConfig.h"

#include "Axiom/Core/Log.h"

#include <fstream>
#include <sstream>

namespace Axiom {

void ServerConfig::Load(const std::string& path) {
	m_Path = path;

	try {
		std::ifstream file(path);
		if (file.good()) {
			m_Root = toml::parse(file, path);
			AX_CORE_INFO("Loaded configuration from {}", path);
		} else {
			AX_CORE_INFO("No configuration file found, creating defaults");
			SetDefaults();
			Save();
		}
	} catch (const toml::parse_error& error) {
		AX_CORE_ERROR("Failed to parse configuration: {}", error.what());
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

void ServerConfig::Reload() {
	AX_CORE_INFO("Reloading configuration from {}", m_Path);
	Load(m_Path);
}

std::string ServerConfig::BindAddress() const {
	return m_Root["network"]["bind-address"].value_or<std::string>("0.0.0.0");
}

uint16_t ServerConfig::Port() const {
	return static_cast<uint16_t>(m_Root["network"]["port"].value_or(25565));
}

std::string ServerConfig::ServerName() const {
	return m_Root["server"]["name"].value_or<std::string>("Axiom Server");
}

std::string ServerConfig::Motd() const {
	return m_Root["server"]["motd"].value_or<std::string>("An Axiom Server");
}

int ServerConfig::MaxPlayers() const {
	return m_Root["server"]["max-players"].value_or(20);
}

int ServerConfig::ViewDistance() const {
	return m_Root["world"]["view-distance"].value_or(10);
}

int ServerConfig::SimulationDistance() const {
	return m_Root["world"]["simulation-distance"].value_or(10);
}

bool ServerConfig::OnlineMode() const {
	return m_Root["network"]["online-mode"].value_or(true);
}

std::string ServerConfig::Difficulty() const {
	return m_Root["world"]["difficulty"].value_or<std::string>("normal");
}

std::string ServerConfig::GameMode() const {
	return m_Root["world"]["game-mode"].value_or<std::string>("survival");
}

int ServerConfig::SpawnProtection() const {
	return m_Root["world"]["spawn-protection"].value_or(16);
}

bool ServerConfig::WhitelistEnabled() const {
	return m_Root["security"]["whitelist"].value_or(false);
}

int ServerConfig::TickRate() const {
	return m_Root["server"]["tick-rate"].value_or(20);
}

int ServerConfig::CompressionThreshold() const {
	return m_Root["network"]["compression-threshold"].value_or(256);
}

std::string ServerConfig::LevelSeed() const {
	return m_Root["world"]["seed"].value_or<std::string>("");
}

std::string ServerConfig::LevelType() const {
	return m_Root["world"]["level-type"].value_or<std::string>("default");
}

bool ServerConfig::Pvp() const {
	return m_Root["world"]["pvp"].value_or(true);
}

bool ServerConfig::EnableCommandBlock() const {
	return m_Root["server"]["enable-command-block"].value_or(false);
}

void ServerConfig::SetDefaults() {
	auto server = toml::table{
		{"name", "Axiom Server"},
		{"motd", "An Axiom Server"},
		{"max-players", 20},
		{"tick-rate", 20},
		{"enable-command-block", false}
	};

	auto network = toml::table{
		{"bind-address", "0.0.0.0"},
		{"port", 25565},
		{"online-mode", true},
		{"compression-threshold", 256}
	};

	auto world = toml::table{
		{"view-distance", 10},
		{"simulation-distance", 10},
		{"difficulty", "normal"},
		{"game-mode", "survival"},
		{"spawn-protection", 16},
		{"seed", ""},
		{"level-type", "default"},
		{"pvp", true}
	};

	auto security = toml::table{
		{"whitelist", false}
	};

	m_Root = toml::table{
		{"server", std::move(server)},
		{"network", std::move(network)},
		{"world", std::move(world)},
		{"security", std::move(security)}
	};
}

}
