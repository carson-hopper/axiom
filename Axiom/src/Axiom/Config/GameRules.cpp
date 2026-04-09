#include "GameRules.h"

#include "Axiom/Core/Log.h"

#include <charconv>

namespace Axiom {

// ---- GameRuleValue -------------------------------------------------

bool GameRuleValue::SetFromString(const std::string& text) {
	if (IsBoolean()) {
		if (text == "true") { m_Value = true; return true; }
		if (text == "false") { m_Value = false; return true; }
		return false;
	}

	int32_t parsed = 0;
	auto [pointer, errorCode] = std::from_chars(
		text.data(), text.data() + text.size(), parsed);
	if (errorCode != std::errc() || pointer != text.data() + text.size())
		return false;

	m_Value = parsed;
	return true;
}

std::string GameRuleValue::ToString() const {
	if (IsBoolean())
		return AsBoolean() ? "true" : "false";
	return std::to_string(AsInteger());
}

// ---- GameRules -----------------------------------------------------

GameRules::GameRules() {
	BootstrapVanillaRules();
}

bool GameRules::Register(const std::string& name, GameRuleValue defaultValue,
	const std::string& description) {

	if (m_Definitions.contains(name))
		return false;

	m_Definitions.emplace(name, RuleDefinition{name, defaultValue, description});
	m_Values.emplace(name, defaultValue);
	return true;
}

GameRuleValue GameRules::Get(const std::string& name) const {
	if (auto iterator = m_Values.find(name); iterator != m_Values.end())
		return iterator->second;

	if (auto iterator = m_Definitions.find(name); iterator != m_Definitions.end())
		return iterator->second.DefaultValue;

	return GameRuleValue{false};
}

bool GameRules::GetBoolean(const std::string& name) const {
	auto value = Get(name);
	return value.IsBoolean() ? value.AsBoolean() : false;
}

int32_t GameRules::GetInteger(const std::string& name) const {
	auto value = Get(name);
	return value.IsInteger() ? value.AsInteger() : 0;
}

bool GameRules::Set(const std::string& name, GameRuleValue value) {
	if (!m_Definitions.contains(name))
		return false;

	auto& definition = m_Definitions.at(name);
	if (definition.DefaultValue.IsBoolean() != value.IsBoolean())
		return false;

	m_Values[name] = value;
	return true;
}

bool GameRules::SetFromString(const std::string& name, const std::string& text) {
	if (!m_Definitions.contains(name))
		return false;

	auto copy = Get(name);
	if (!copy.SetFromString(text))
		return false;

	m_Values[name] = copy;
	return true;
}

bool GameRules::Has(const std::string& name) const {
	return m_Definitions.contains(name);
}

void GameRules::ResetToDefault(const std::string& name) {
	if (auto iterator = m_Definitions.find(name); iterator != m_Definitions.end())
		m_Values[name] = iterator->second.DefaultValue;
}

void GameRules::ResetAllToDefaults() {
	for (auto& [name, definition] : m_Definitions)
		m_Values[name] = definition.DefaultValue;
}

std::unordered_map<std::string, std::string> GameRules::Serialize() const {
	std::unordered_map<std::string, std::string> result;
	for (auto& [name, value] : m_Values)
		result[name] = value.ToString();
	return result;
}

void GameRules::Deserialize(const std::unordered_map<std::string, std::string>& data) {
	for (auto& [name, text] : data) {
		if (!m_Definitions.contains(name))
			continue;

		auto copy = m_Definitions.at(name).DefaultValue;
		if (copy.SetFromString(text))
			m_Values[name] = copy;
		else
			AX_CORE_WARN("Failed to parse game rule '{}' value '{}'", name, text);
	}
}

std::vector<std::string> GameRules::RuleNames() const {
	std::vector<std::string> names;
	names.reserve(m_Definitions.size());
	for (auto& [name, definition] : m_Definitions)
		names.push_back(name);
	return names;
}

const GameRules::RuleDefinition* GameRules::Definition(const std::string& name) const {
	auto iterator = m_Definitions.find(name);
	if (iterator == m_Definitions.end())
		return nullptr;
	return &iterator->second;
}

void GameRules::BootstrapVanillaRules() {
	// Boolean rules
	Register("keepInventory", GameRuleValue{false},
		"Players keep inventory on death");
	Register("doDaylightCycle", GameRuleValue{true},
		"Advance time of day");
	Register("doMobSpawning", GameRuleValue{true},
		"Natural mob spawning");
	Register("doFireTick", GameRuleValue{true},
		"Fire spreads and extinguishes");
	Register("mobGriefing", GameRuleValue{true},
		"Mobs can modify the world");
	Register("naturalRegeneration", GameRuleValue{true},
		"Players regenerate health when saturation is high");
	Register("doWeatherCycle", GameRuleValue{true},
		"Weather changes over time");
	Register("pvp", GameRuleValue{true},
		"Players can damage each other");
	Register("commandBlockOutput", GameRuleValue{true},
		"Command blocks notify admins of output");
	Register("doImmediateRespawn", GameRuleValue{false},
		"Players respawn without the death screen");
	Register("doTileDrops", GameRuleValue{true},
		"Broken blocks drop items");
	Register("doEntityDrops", GameRuleValue{true},
		"Entities drop items on death");
	Register("doInsomnia", GameRuleValue{true},
		"Phantoms spawn when players skip sleeping");
	Register("doPatrolSpawning", GameRuleValue{true},
		"Pillager patrols can spawn");
	Register("doTraderSpawning", GameRuleValue{true},
		"Wandering traders can spawn");
	Register("doWardenSpawning", GameRuleValue{true},
		"Wardens can spawn");
	Register("drowningDamage", GameRuleValue{true},
		"Players take drowning damage");
	Register("fallDamage", GameRuleValue{true},
		"Players take fall damage");
	Register("fireDamage", GameRuleValue{true},
		"Players take fire damage");
	Register("freezeDamage", GameRuleValue{true},
		"Players take freeze damage");
	Register("forgiveDeadPlayers", GameRuleValue{true},
		"Mobs forget aggro when player dies");
	Register("universalAnger", GameRuleValue{false},
		"Angered neutral mobs attack all nearby players");
	Register("announceAdvancements", GameRuleValue{true},
		"Advancement completions are announced in chat");
	Register("disableElytraMovementCheck", GameRuleValue{false},
		"Disable server-side elytra speed check");
	Register("disableRaids", GameRuleValue{false},
		"Raids cannot start");
	Register("doLimitedCrafting", GameRuleValue{false},
		"Recipes must be unlocked before crafting");
	Register("logAdminCommands", GameRuleValue{true},
		"Log admin commands to server log");
	Register("reducedDebugInfo", GameRuleValue{false},
		"Send reduced debug info to clients");
	Register("sendCommandFeedback", GameRuleValue{true},
		"Command output is sent to the executing player");
	Register("showDeathMessages", GameRuleValue{true},
		"Death messages appear in chat");
	Register("spectatorsGenerateChunks", GameRuleValue{true},
		"Spectators can cause chunk generation");
	Register("blockExplosionDropDecay", GameRuleValue{true},
		"Some blocks destroyed by explosions drop nothing");
	Register("mobExplosionDropDecay", GameRuleValue{true},
		"Some blocks destroyed by mob explosions drop nothing");
	Register("tntExplosionDropDecay", GameRuleValue{false},
		"Some blocks destroyed by TNT drop nothing");
	Register("globalSoundEvents", GameRuleValue{true},
		"Global sound events are audible to all players");
	Register("lavaSourceConversion", GameRuleValue{false},
		"Lava can become a source block");
	Register("waterSourceConversion", GameRuleValue{true},
		"Water can become a source block");
	Register("projectilesCanBreakBlocks", GameRuleValue{true},
		"Projectiles can break certain blocks");
	Register("enderPearlsVanishOnDeath", GameRuleValue{true},
		"Thrown ender pearls vanish when the thrower dies");

	// Integer rules
	Register("randomTickSpeed", GameRuleValue{int32_t(3)},
		"Random ticks per chunk section per game tick");
	Register("spawnRadius", GameRuleValue{int32_t(10)},
		"Radius around spawn point for initial placement");
	Register("maxEntityCramming", GameRuleValue{int32_t(24)},
		"Maximum entities in one block space");
	Register("maxCommandChainLength", GameRuleValue{int32_t(65536)},
		"Maximum command chain length");
	Register("playersNetherPortalDefaultDelay", GameRuleValue{int32_t(80)},
		"Ticks before portal transition in survival");
	Register("playersNetherPortalCreativeDelay", GameRuleValue{int32_t(1)},
		"Ticks before portal transition in creative");
	Register("playersSleepingPercentage", GameRuleValue{int32_t(100)},
		"Percentage of players needed to skip the night");
	Register("snowAccumulationHeight", GameRuleValue{int32_t(1)},
		"Maximum snow layer height from snowfall");
	Register("commandModificationBlockLimit", GameRuleValue{int32_t(32768)},
		"Maximum blocks modified by a single command");
	Register("maxCommandForkCount", GameRuleValue{int32_t(65536)},
		"Maximum number of forked command executions");
	Register("spawnChunkRadius", GameRuleValue{int32_t(2)},
		"Chunk radius kept loaded around spawn");
}

}
