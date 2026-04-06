#pragma once

#include "Axiom/Core/Base.h"

#include <yaml-cpp/yaml.h>

#include <cstdint>
#include <string>

namespace Axiom {

	class ServerConfig {
	public:
		void Load(const std::string& path);
		void Save();

		std::string ServerName() const;
		uint16_t Port() const;
		int MaxPlayers() const;
		int ViewDistance() const;
		std::string Motd() const;
		bool OnlineMode() const;
		int TickRate() const;

	private:
		void SetDefaults();

		YAML::Node m_Root;
		std::string m_Path;
	};

}
