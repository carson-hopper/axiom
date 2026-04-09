#include "AdminFileStore.h"

#include "Axiom/Core/Log.h"

#include <algorithm>
#include <filesystem>
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
	auto data = LoadJsonFile("ops.json");
	for (auto& entry : data) {
		OpEntry operation;
		operation.Uuid = entry.value("uuid", "");
		operation.Name = entry.value("name", "");
		operation.Level = entry.value("level", 4);
		operation.BypassesPlayerLimit = entry.value("bypassesPlayerLimit", false);
		m_Ops.push_back(std::move(operation));
	}
}

void AdminFileStore::SaveOps() {
	nlohmann::json data = nlohmann::json::array();
	for (auto& entry : m_Ops) {
		data.push_back({
			{"uuid", entry.Uuid},
			{"name", entry.Name},
			{"level", entry.Level},
			{"bypassesPlayerLimit", entry.BypassesPlayerLimit}
		});
	}
	SaveJsonFile("ops.json", data);
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
	auto data = LoadJsonFile("whitelist.json");
	for (auto& entry : data) {
		WhitelistEntry whitelistEntry;
		whitelistEntry.Uuid = entry.value("uuid", "");
		whitelistEntry.Name = entry.value("name", "");
		m_Whitelist.push_back(std::move(whitelistEntry));
	}
}

void AdminFileStore::SaveWhitelist() {
	nlohmann::json data = nlohmann::json::array();
	for (auto& entry : m_Whitelist) {
		data.push_back({
			{"uuid", entry.Uuid},
			{"name", entry.Name}
		});
	}
	SaveJsonFile("whitelist.json", data);
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
	auto data = LoadJsonFile("banned-players.json");
	for (auto& entry : data) {
		BannedPlayerEntry bannedEntry;
		bannedEntry.Uuid = entry.value("uuid", "");
		bannedEntry.Name = entry.value("name", "");
		bannedEntry.Created = entry.value("created", "");
		bannedEntry.Source = entry.value("source", "");
		bannedEntry.Expires = entry.value("expires", "forever");
		bannedEntry.Reason = entry.value("reason", "Banned by an operator");
		m_BannedPlayers.push_back(std::move(bannedEntry));
	}
}

void AdminFileStore::SaveBannedPlayers() {
	nlohmann::json data = nlohmann::json::array();
	for (auto& entry : m_BannedPlayers) {
		data.push_back({
			{"uuid", entry.Uuid},
			{"name", entry.Name},
			{"created", entry.Created},
			{"source", entry.Source},
			{"expires", entry.Expires},
			{"reason", entry.Reason}
		});
	}
	SaveJsonFile("banned-players.json", data);
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
	auto data = LoadJsonFile("banned-ips.json");
	for (auto& entry : data) {
		BannedIpEntry bannedEntry;
		bannedEntry.Ip = entry.value("ip", "");
		bannedEntry.Created = entry.value("created", "");
		bannedEntry.Source = entry.value("source", "");
		bannedEntry.Expires = entry.value("expires", "forever");
		bannedEntry.Reason = entry.value("reason", "Banned by an operator");
		m_BannedIps.push_back(std::move(bannedEntry));
	}
}

void AdminFileStore::SaveBannedIps() {
	nlohmann::json data = nlohmann::json::array();
	for (auto& entry : m_BannedIps) {
		data.push_back({
			{"ip", entry.Ip},
			{"created", entry.Created},
			{"source", entry.Source},
			{"expires", entry.Expires},
			{"reason", entry.Reason}
		});
	}
	SaveJsonFile("banned-ips.json", data);
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

void AdminFileStore::LoadUserCache() {
	m_UserCache.clear();
	auto data = LoadJsonFile("usercache.json");
	for (auto& entry : data) {
		UserCacheEntry cacheEntry;
		cacheEntry.Uuid = entry.value("uuid", "");
		cacheEntry.Name = entry.value("name", "");
		cacheEntry.ExpiresOn = entry.value("expiresOn", "");
		m_UserCache.push_back(std::move(cacheEntry));
	}
}

void AdminFileStore::SaveUserCache() {
	nlohmann::json data = nlohmann::json::array();
	for (auto& entry : m_UserCache) {
		data.push_back({
			{"uuid", entry.Uuid},
			{"name", entry.Name},
			{"expiresOn", entry.ExpiresOn}
		});
	}
	SaveJsonFile("usercache.json", data);
}

std::optional<UserCacheEntry> AdminFileStore::FindByUuid(const std::string& uuid) const {
	for (auto& entry : m_UserCache) {
		if (entry.Uuid == uuid)
			return entry;
	}
	return std::nullopt;
}

std::optional<UserCacheEntry> AdminFileStore::FindByName(const std::string& name) const {
	for (auto& entry : m_UserCache) {
		if (entry.Name == name)
			return entry;
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

nlohmann::json AdminFileStore::LoadJsonFile(const std::string& filename) {
	auto path = FilePath(filename);
	try {
		std::ifstream file(path);
		if (!file.good()) {
			AX_CORE_INFO("Creating empty {}", filename);
			SaveJsonFile(filename, nlohmann::json::array());
			return nlohmann::json::array();
		}
		auto data = nlohmann::json::parse(file, nullptr, false);
		if (data.is_discarded() || !data.is_array()) {
			AX_CORE_WARN("Invalid JSON in {}, resetting to empty", filename);
			return nlohmann::json::array();
		}
		AX_CORE_INFO("Loaded {} entries from {}", data.size(), filename);
		return data;
	} catch (const std::exception& exception) {
		AX_CORE_ERROR("Failed to load {}: {}", filename, exception.what());
		return nlohmann::json::array();
	}
}

void AdminFileStore::SaveJsonFile(const std::string& filename, const nlohmann::json& data) {
	auto path = FilePath(filename);
	try {
		std::ofstream file(path);
		file << data.dump(2) << std::endl;
	} catch (const std::exception& exception) {
		AX_CORE_ERROR("Failed to save {}: {}", filename, exception.what());
	}
}

}
