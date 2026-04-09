#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Core/Math.h"

#include <optional>
#include <string>

namespace Axiom {

class AdminFileStore;
class ServerConfig;
class Player;

/**
 * Stateless enforcement helpers that query the
 * AdminFileStore and ServerConfig to make access
 * control decisions.
 */
namespace AdminEnforcement {

	/**
	 * Returns true when the player UUID is on the
	 * whitelist, or when the whitelist is disabled.
	 */
	bool CheckWhitelist(const std::string& uuid,
		const AdminFileStore& adminFiles);

	/**
	 * Returns the ban reason if the player UUID or
	 * connecting IP is banned, otherwise nullopt.
	 */
	std::optional<std::string> CheckBanned(
		const std::string& uuid, const std::string& ipAddress,
		const AdminFileStore& adminFiles);

	/**
	 * Returns true when the player is allowed to modify
	 * the block at the given position. Returns false when
	 * the block falls inside the spawn protection radius
	 * and the player is not an operator.
	 */
	bool CanModifyBlock(const Vector3& position,
		const Ref<Player>& player, const ServerConfig& config);

	/**
	 * Returns the operator permission level for the
	 * given player UUID, or 0 when the player is not
	 * an operator.
	 */
	int GetPermissionLevel(const std::string& uuid,
		const AdminFileStore& adminFiles);

}

}
