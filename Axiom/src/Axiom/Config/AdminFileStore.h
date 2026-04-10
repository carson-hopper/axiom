#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Core/Time.h"

#include <nlohmann/json.hpp>
#include <toml++/toml.hpp>

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace Axiom {

/**
 * Entry in the operators list (ops.toml).
 * The UUID is the stable identifier; player
 * names live in usercache.json.
 */
struct OpEntry {
	std::string Uuid;
	int Level = 4;
	bool BypassesPlayerLimit = false;
};

/**
 * Entry in the whitelist (whitelist.json).
 */
struct WhitelistEntry {
	std::string Uuid;
	std::string Name;
};

/**
 * Entry in banned-players.json.
 */
struct BannedPlayerEntry {
	std::string Uuid;
	std::string Name;
	std::string Created;
	std::string Source;
	std::string Expires;
	std::string Reason;
};

/**
 * Entry in banned-ips.json.
 */
struct BannedIpEntry {
	std::string Ip;
	std::string Created;
	std::string Source;
	std::string Expires;
	std::string Reason;
};

/**
 * Entry in usercache.toml.
 *
 * ExpiresOn is a wall-clock instant; lookups
 * ignore entries whose expiry has passed. A
 * default-constructed Time means "never".
 */
struct UserCacheEntry {
	std::string Uuid;
	std::string Name;
	Time ExpiresOn;
};

/**
 * Manages the JSON-backed admin and security files
 * required by the Minecraft server spec: ops.json,
 * whitelist.json, banned-players.json, banned-ips.json,
 * and usercache.json.
 */
class AdminFileStore {
public:
	explicit AdminFileStore(const std::string& directory = ".");

	/** Load all admin files from disk. Creates empty defaults if missing. */
	void LoadAll();

	/** Save all admin files to disk. */
	void SaveAll();

	// ---- Operators ------------------------------------------------

	void LoadOps();
	void SaveOps();
	bool IsOp(const std::string& uuid) const;
	int OpLevel(const std::string& uuid) const;
	void AddOp(const OpEntry& entry);
	void RemoveOp(const std::string& uuid);
	const std::vector<OpEntry>& Ops() const { return m_Ops; }

	// ---- Whitelist ------------------------------------------------

	void LoadWhitelist();
	void SaveWhitelist();
	bool IsWhitelisted(const std::string& uuid) const;
	void AddWhitelistEntry(const WhitelistEntry& entry);
	void RemoveWhitelistEntry(const std::string& uuid);
	const std::vector<WhitelistEntry>& Whitelist() const { return m_Whitelist; }

	// ---- Banned players -------------------------------------------

	void LoadBannedPlayers();
	void SaveBannedPlayers();
	bool IsPlayerBanned(const std::string& uuid) const;
	void BanPlayer(const BannedPlayerEntry& entry);
	void UnbanPlayer(const std::string& uuid);
	const std::vector<BannedPlayerEntry>& BannedPlayers() const { return m_BannedPlayers; }

	// ---- Banned IPs -----------------------------------------------

	void LoadBannedIps();
	void SaveBannedIps();
	bool IsIpBanned(const std::string& ipAddress) const;
	void BanIp(const BannedIpEntry& entry);
	void UnbanIp(const std::string& ipAddress);
	const std::vector<BannedIpEntry>& BannedIps() const { return m_BannedIps; }

	// ---- User cache -----------------------------------------------

	void LoadUserCache();
	void SaveUserCache();
	std::optional<UserCacheEntry> FindByUuid(const std::string& uuid) const;
	std::optional<UserCacheEntry> FindByName(const std::string& name) const;
	void UpdateCache(const UserCacheEntry& entry);

private:
	std::string FilePath(const std::string& filename) const;
	nlohmann::json LoadJsonFile(const std::string& filename);
	void SaveJsonFile(const std::string& filename, const nlohmann::json& data);
	toml::table LoadTomlFile(const std::string& filename);
	void SaveTomlFile(const std::string& filename, const toml::table& data);

	std::string m_Directory;
	std::vector<OpEntry> m_Ops;
	std::vector<WhitelistEntry> m_Whitelist;
	std::vector<BannedPlayerEntry> m_BannedPlayers;
	std::vector<BannedIpEntry> m_BannedIps;
	std::vector<UserCacheEntry> m_UserCache;
};

}
