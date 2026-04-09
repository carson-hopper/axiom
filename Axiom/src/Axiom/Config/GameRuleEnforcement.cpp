#include "GameRuleEnforcement.h"

#include "Axiom/Config/GameRules.h"

namespace Axiom {

	GameRuleEnforcement::GameRuleEnforcement(GameRules& rules)
		: m_Rules(rules) {}

	bool GameRuleEnforcement::ShouldAdvanceTime() const {
		return m_Rules.GetBoolean("doDaylightCycle");
	}

	bool GameRuleEnforcement::ShouldSpawnMobs() const {
		return m_Rules.GetBoolean("doMobSpawning");
	}

	bool GameRuleEnforcement::ShouldKeepInventory() const {
		return m_Rules.GetBoolean("keepInventory");
	}

	bool GameRuleEnforcement::ShouldDropTileLoot() const {
		return m_Rules.GetBoolean("doTileDrops");
	}

	bool GameRuleEnforcement::ShouldDropEntityLoot() const {
		return m_Rules.GetBoolean("doEntityDrops");
	}

	bool GameRuleEnforcement::ShouldSpreadFire() const {
		return m_Rules.GetBoolean("doFireTick");
	}

	bool GameRuleEnforcement::AllowMobGriefing() const {
		return m_Rules.GetBoolean("mobGriefing");
	}

	bool GameRuleEnforcement::AllowNaturalRegen() const {
		return m_Rules.GetBoolean("naturalRegeneration");
	}

	bool GameRuleEnforcement::AllowPvp() const {
		return m_Rules.GetBoolean("pvp");
	}

	bool GameRuleEnforcement::ShouldWeatherCycle() const {
		return m_Rules.GetBoolean("doWeatherCycle");
	}

	int32_t GameRuleEnforcement::RandomTickSpeed() const {
		return m_Rules.GetInteger("randomTickSpeed");
	}

	int32_t GameRuleEnforcement::SpawnRadius() const {
		return m_Rules.GetInteger("spawnRadius");
	}

}
