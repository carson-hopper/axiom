#pragma once

#include <cstdint>

namespace Axiom {

	class GameRules;

	/**
	 * Wires game rules into active gameplay
	 * by providing named boolean and integer
	 * queries backed by the rule store.
	 */
	class GameRuleEnforcement {
	public:
		explicit GameRuleEnforcement(GameRules& rules);

		bool ShouldAdvanceTime() const;
		bool ShouldSpawnMobs() const;
		bool ShouldKeepInventory() const;
		bool ShouldDropTileLoot() const;
		bool ShouldDropEntityLoot() const;
		bool ShouldSpreadFire() const;
		bool AllowMobGriefing() const;
		bool AllowNaturalRegen() const;
		bool AllowPvp() const;
		bool ShouldWeatherCycle() const;
		int32_t RandomTickSpeed() const;
		int32_t SpawnRadius() const;

	private:
		GameRules& m_Rules;
	};

}
