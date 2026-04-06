#pragma once

#include "Axiom/Core/Base.h"

#include <nlohmann/json.hpp>

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace Axiom {

	struct GameProfile {
		std::string uuid;
		std::string name;
		nlohmann::json properties;
	};

	class MojangAuth {
	public:
		/**
		 * Verifies a player's session with Mojang's sessionserver.
		 *
		 * @param username The player's username
		 * @param serverHash The computed server hash
		 * @return The player's GameProfile if verified, or nullopt
		 */
		static std::optional<GameProfile> HasJoined(
			const std::string& username,
			const std::string& serverHash);
	};

}
