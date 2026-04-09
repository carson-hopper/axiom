#include "Axiom/Network/AdminEnforcement.h"

#include "Axiom/Config/AdminFileStore.h"
#include "Axiom/Config/ServerConfig.h"
#include "Axiom/Environment/Entity/Player.h"

#include <cmath>

namespace Axiom::AdminEnforcement {

bool CheckWhitelist(const std::string& uuid,
	const AdminFileStore& adminFiles) {
	return adminFiles.IsWhitelisted(uuid);
}

std::optional<std::string> CheckBanned(
	const std::string& uuid, const std::string& ipAddress,
	const AdminFileStore& adminFiles) {

	if (adminFiles.IsPlayerBanned(uuid)) {
		for (auto& entry : adminFiles.BannedPlayers()) {
			if (entry.Uuid == uuid) {
				return entry.Reason.empty()
					? std::optional<std::string>("Banned from this server")
					: std::optional<std::string>(entry.Reason);
			}
		}
		return "Banned from this server";
	}

	if (adminFiles.IsIpBanned(ipAddress)) {
		for (auto& entry : adminFiles.BannedIps()) {
			if (entry.Ip == ipAddress) {
				return entry.Reason.empty()
					? std::optional<std::string>("Your IP is banned")
					: std::optional<std::string>(entry.Reason);
			}
		}
		return "Your IP is banned";
	}

	return std::nullopt;
}

bool CanModifyBlock(const Vector3& position,
	const Ref<Player>& player, const ServerConfig& config) {

	int spawnProtection = config.SpawnProtection();
	if (spawnProtection <= 0) {
		return true;
	}

	// Operators bypass spawn protection
	if (player) {
		// Check if the player is an op via the Application's
		// admin files. For simplicity we check permission
		// directly on the player object.
		if (player->HasPermission("axiom.bypass.spawnprotection")) {
			return true;
		}
	}

	// Check distance from world spawn (assumed 0, 0)
	double distanceX = std::abs(position.x);
	double distanceZ = std::abs(position.z);
	double maxDistance = static_cast<double>(spawnProtection);

	return distanceX > maxDistance || distanceZ > maxDistance;
}

int GetPermissionLevel(const std::string& uuid,
	const AdminFileStore& adminFiles) {
	return adminFiles.OpLevel(uuid);
}

}
