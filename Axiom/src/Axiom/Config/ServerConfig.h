#pragma once

#include "Axiom/Core/Base.h"

#include <toml++/toml.hpp>

#include <cstdint>
#include <string>

namespace Axiom {

/**
 * Primary configuration loaded from server.toml.
 *
 * This is the canonical authoring format. The server
 * also exposes a compatibility layer through
 * DedicatedProperties for spec-facing settings.
 */
class ServerConfig {
public:
	void Load(const std::string& path);
	void Save();
	void Reload();

	/** Bind address for the TCP listener. */
	std::string BindAddress() const;

	/** TCP port. */
	int32_t Port() const;

	/** Display name shown in the server list. */
	std::string ServerName() const;

	/** MOTD shown in the server list. */
	std::string Motd() const;

	/** Maximum concurrent players. */
	int MaxPlayers() const;

	/** Chunk render distance. */
	int ViewDistance() const;

	/** Entity simulation distance. */
	int SimulationDistance() const;

	/** Online (Mojang auth) or offline mode. */
	bool OnlineMode() const;

	/** Difficulty level (peaceful/easy/normal/hard). */
	std::string Difficulty() const;

	/** Default game mode (survival/creative/adventure/spectator). */
	std::string GameMode() const;

	/** Radius around spawn protected from non-ops. */
	int SpawnProtection() const;

	/** Whether the whitelist is enforced. */
	bool WhitelistEnabled() const;

	/** Server tick rate. */
	int TickRate() const;

	/** Compression threshold in bytes (-1 to disable). */
	int CompressionThreshold() const;

	/** Level seed. */
	std::string LevelSeed() const;

	/** Level type (default/flat/amplified). */
	std::string LevelType() const;

	/** Whether PvP is enabled. */
	bool Pvp() const;

	/** Whether command blocks are enabled. */
	bool EnableCommandBlock() const;

	/** Access to the raw TOML table for extensibility. */
	const toml::table& Root() const { return m_Root; }

private:
	void SetDefaults();

	toml::table m_Root;
	std::string m_Path;
};

}
