#pragma once

#include "Axiom/Config/ServerConfig.h"

#include <string>
#include <unordered_map>

namespace Axiom {

/**
 * Compatibility layer that reads and writes the
 * vanilla server.properties key-value format.
 *
 * The canonical source of truth is ServerConfig (TOML).
 * This class provides two-way bridging for settings
 * required by the spec: bind address, port, online mode,
 * MOTD, max players, view/simulation distance, spawn
 * protection, whitelist, and difficulty.
 */
class DedicatedProperties {
public:
	/** Load from a server.properties file. */
	void Load(const std::string& path);

	/** Save current state to a server.properties file. */
	void Save(const std::string& path) const;

	/**
	 * Import values from a server.properties file into
	 * a ServerConfig. Existing TOML values take precedence;
	 * only keys absent in the config are imported.
	 */
	void ImportInto(ServerConfig& config) const;

	/**
	 * Export the current ServerConfig values to
	 * server.properties format.
	 */
	void ExportFrom(const ServerConfig& config);

	/** Get a raw property value. */
	std::string Get(const std::string& key, const std::string& fallback = "") const;

	/** Set a raw property value. */
	void Set(const std::string& key, const std::string& value);

private:
	std::unordered_map<std::string, std::string> m_Properties;
};

}
