#include "AdminFileStore.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Core/Time.h"

#include <algorithm>
#include <fstream>

namespace Axiom {

AdminFileStore::AdminFileStore(const std::string& directory)
	: m_Directory(directory) {
}

void AdminFileStore::LoadAll() {
	LoadOps();
	LoadWhitelist();
	LoadBannedPlayers();
	LoadBannedIps();
	LoadUserCache();
}

void AdminFileStore::SaveAll() {
	SaveOps();
	SaveWhitelist();
	SaveBannedPlayers();
	SaveBannedIps();
	SaveUserCache();
}

// ---- Operators ---------------------------------------------------------

void AdminFileStore::LoadOps() {
	m_Ops.clear();

	auto data = LoadTomlFile("ops.toml");
	for (const auto& [key, value] : data) {
		const auto* entryTable = value.as_table();
		if (!entryTable) {
			continue;
		}
		OpEntry operation;
		operation.Uuid = std::string(key.str());
		operation.Level = (*entryTable)["level"].value_or(4);
		operation.BypassesPlayerLimit = (*entryTable)["bypasses-player-limit"].value_or(false);
		m_Ops.push_back(std::move(operation));
	}
}

void AdminFileStore::SaveOps() {
	toml::table root;
	for (const auto& [Uuid, Level, BypassesPlayerLimit] : m_Ops) {
		toml::table row;
		row.insert("bypasses-player-limit", BypassesPlayerLimit);
		row.insert("level", static_cast<int64_t>(Level));
		root.insert(Uuid, std::move(row));
	}
	SaveTomlFile("ops.toml", root);
}

bool AdminFileStore::IsOp(const std::string& uuid) const {
	return std::ranges::any_of(m_Ops, [&](auto& entry) {
		return entry.Uuid == uuid;
	});
}

int AdminFileStore::OpLevel(const std::string& uuid) const {
	for (auto& entry : m_Ops) {
		if (entry.Uuid == uuid)
			return entry.Level;
	}
	return 0;
}

void AdminFileStore::AddOp(const OpEntry& entry) {
	RemoveOp(entry.Uuid);
	m_Ops.push_back(entry);
	SaveOps();
}

void AdminFileStore::RemoveOp(const std::string& uuid) {
	std::erase_if(m_Ops, [&](auto& entry) { return entry.Uuid == uuid; });
	SaveOps();
}

// ---- Whitelist ---------------------------------------------------------

void AdminFileStore::LoadWhitelist() {
	m_Whitelist.clear();

	auto data = LoadTomlFile("whitelist.toml");
	for (const auto& [key, value] : data) {
		const auto* entryTable = value.as_table();
		if (!entryTable) {
			continue;
		}
		WhitelistEntry whitelistEntry;
		whitelistEntry.Uuid = std::string(key.str());
		whitelistEntry.Name = (*entryTable)["name"].value_or<std::string>("");
		m_Whitelist.push_back(std::move(whitelistEntry));
	}
}

void AdminFileStore::SaveWhitelist() {
	toml::table root;
	for (const auto& [Uuid, Name] : m_Whitelist) {
		toml::table row;
		row.insert("name", Name);
		root.insert(Uuid, std::move(row));
	}
	SaveTomlFile("whitelist.toml", root);
}

bool AdminFileStore::IsWhitelisted(const std::string& uuid) const {
	return std::ranges::any_of(m_Whitelist, [&](auto& entry) {
		return entry.Uuid == uuid;
	});
}

void AdminFileStore::AddWhitelistEntry(const WhitelistEntry& entry) {
	RemoveWhitelistEntry(entry.Uuid);
	m_Whitelist.push_back(entry);
	SaveWhitelist();
}

void AdminFileStore::RemoveWhitelistEntry(const std::string& uuid) {
	std::erase_if(m_Whitelist, [&](auto& entry) { return entry.Uuid == uuid; });
	SaveWhitelist();
}

// ---- Banned players ----------------------------------------------------

void AdminFileStore::LoadBannedPlayers() {
	m_BannedPlayers.clear();

	auto data = LoadTomlFile("banned-players.toml");
	for (const auto& [key, value] : data) {
		const auto* entryTable = value.as_table();
		if (!entryTable) {
			continue;
		}
		BannedPlayerEntry bannedEntry;
		bannedEntry.Uuid = std::string(key.str());
		bannedEntry.Name = (*entryTable)["name"].value_or<std::string>("");
		bannedEntry.Created = (*entryTable)["created"].value_or<std::string>("");
		bannedEntry.Source = (*entryTable)["source"].value_or<std::string>("");
		bannedEntry.Expires = (*entryTable)["expires"].value_or<std::string>("forever");
		bannedEntry.Reason = (*entryTable)["reason"].value_or<std::string>("Banned by an operator");
		m_BannedPlayers.push_back(std::move(bannedEntry));
	}
}

void AdminFileStore::SaveBannedPlayers() {
	toml::table root;
	for (const auto& [Uuid, Name, Created, Source, Expires, Reason] : m_BannedPlayers) {
		toml::table row;
		row.insert("created", Created);
		row.insert("expires", Expires);
		row.insert("name", Name);
		row.insert("reason", Reason);
		row.insert("source", Source);
		root.insert(Uuid, std::move(row));
	}
	SaveTomlFile("banned-players.toml", root);
}

bool AdminFileStore::IsPlayerBanned(const std::string& uuid) const {
	return std::ranges::any_of(m_BannedPlayers, [&](auto& entry) {
		return entry.Uuid == uuid;
	});
}

void AdminFileStore::BanPlayer(const BannedPlayerEntry& entry) {
	UnbanPlayer(entry.Uuid);
	m_BannedPlayers.push_back(entry);
	SaveBannedPlayers();
}

void AdminFileStore::UnbanPlayer(const std::string& uuid) {
	std::erase_if(m_BannedPlayers, [&](auto& entry) { return entry.Uuid == uuid; });
	SaveBannedPlayers();
}

// ---- Banned IPs --------------------------------------------------------

void AdminFileStore::LoadBannedIps() {
	m_BannedIps.clear();

	auto data = LoadTomlFile("banned-ips.toml");
	for (const auto& [key, value] : data) {
		const auto* entryTable = value.as_table();
		if (!entryTable) {
			continue;
		}
		BannedIpEntry bannedEntry;
		bannedEntry.Ip = std::string(key.str());
		bannedEntry.Created = (*entryTable)["created"].value_or<std::string>("");
		bannedEntry.Source = (*entryTable)["source"].value_or<std::string>("");
		bannedEntry.Expires = (*entryTable)["expires"].value_or<std::string>("forever");
		bannedEntry.Reason = (*entryTable)["reason"].value_or<std::string>("Banned by an operator");
		m_BannedIps.push_back(std::move(bannedEntry));
	}
}

void AdminFileStore::SaveBannedIps() {
	toml::table root;
	for (const auto& [Ip, Created, Source, Expires, Reason] : m_BannedIps) {
		toml::table row;
		row.insert("created", Created);
		row.insert("expires", Expires);
		row.insert("reason", Reason);
		row.insert("source", Source);
		root.insert(Ip, std::move(row));
	}
	SaveTomlFile("banned-ips.toml", root);
}

bool AdminFileStore::IsIpBanned(const std::string& ipAddress) const {
	return std::ranges::any_of(m_BannedIps, [&](auto& entry) {
		return entry.Ip == ipAddress;
	});
}

void AdminFileStore::BanIp(const BannedIpEntry& entry) {
	UnbanIp(entry.Ip);
	m_BannedIps.push_back(entry);
	SaveBannedIps();
}

void AdminFileStore::UnbanIp(const std::string& ipAddress) {
	std::erase_if(m_BannedIps, [&](auto& entry) { return entry.Ip == ipAddress; });
	SaveBannedIps();
}

// ---- User cache --------------------------------------------------------

namespace {
	bool IsCacheEntryValid(const UserCacheEntry& entry) {
		if (entry.ExpiresOn.IsZero()) {
			return true;
		}
		return !entry.ExpiresOn.HasPassed();
	}
}

void AdminFileStore::LoadUserCache() {
	m_UserCache.clear();

	auto data = LoadTomlFile("usercache.toml");

	size_t pruned = 0;
	for (const auto& [key, value] : data) {
		const auto* entryTable = value.as_table();
		if (!entryTable) {
			continue;
		}
		UserCacheEntry cacheEntry;
		cacheEntry.Uuid = std::string(key.str());
		cacheEntry.Name = (*entryTable)["name"].value_or<std::string>("");
		cacheEntry.ExpiresOn = Time((*entryTable)["expires-on"].value_or<int64_t>(0));
		if (!IsCacheEntryValid(cacheEntry)) {
			pruned++;
			continue;
		}
		m_UserCache.push_back(std::move(cacheEntry));
	}

	if (pruned > 0) {
		AX_CORE_INFO("Pruned {} expired usercache entries", pruned);
		SaveUserCache();
	}
}

void AdminFileStore::SaveUserCache() {
	toml::table root;
	for (const auto& [Uuid, Name, ExpiresOn] : m_UserCache) {
		toml::table row;
		row.insert("expires-on", ExpiresOn.UnixSeconds());
		row.insert("name", Name);
		root.insert(Uuid, std::move(row));
	}
	SaveTomlFile("usercache.toml", root);
}

std::optional<UserCacheEntry> AdminFileStore::FindByUuid(const std::string& uuid) const {
	for (const auto& entry : m_UserCache) {
		if (entry.Uuid == uuid && IsCacheEntryValid(entry)) {
			return entry;
		}
	}
	return std::nullopt;
}

std::optional<UserCacheEntry> AdminFileStore::FindByName(const std::string& name) const {
	for (const auto& entry : m_UserCache) {
		if (entry.Name == name && IsCacheEntryValid(entry)) {
			return entry;
		}
	}
	return std::nullopt;
}

void AdminFileStore::UpdateCache(const UserCacheEntry& entry) {
	std::erase_if(m_UserCache, [&](auto& existing) {
		return existing.Uuid == entry.Uuid;
	});
	m_UserCache.push_back(entry);
	SaveUserCache();
}

// ---- Helpers -----------------------------------------------------------

std::string AdminFileStore::FilePath(const std::string& filename) const {
	return m_Directory + "/" + filename;
}

toml::table AdminFileStore::LoadTomlFile(const std::string& filename) {
	auto path = FilePath(filename);
	try {
		std::ifstream file(path);
		if (!file.good()) {
			AX_CORE_INFO("Creating empty {}", filename);
			SaveTomlFile(filename, toml::table{});
			return toml::table{};
		}
		auto data = toml::parse(file, path);
		AX_CORE_INFO("Loaded {}", filename);
		return data;
	} catch (const toml::parse_error& error) {
		AX_CORE_WARN("Invalid TOML in {}, resetting: {}", filename, error.description());
		return toml::table{};
	} catch (const std::exception& exception) {
		AX_CORE_ERROR("Failed to load {}: {}", filename, exception.what());
		return toml::table{};
	}
}

void AdminFileStore::SaveTomlFile(const std::string& filename, const toml::table& data) {
	auto path = FilePath(filename);
	try {
		std::ofstream file(path);
		file << data;
	} catch (const std::exception& exception) {
		AX_CORE_ERROR("Failed to save {}: {}", filename, exception.what());
	}
}

}
