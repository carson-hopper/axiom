#pragma once

#include "Axiom/Core/Base.h"

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <variant>

namespace Axiom {

/**
 * Typed game rule value. Each rule is either a
 * boolean or a bounded integer.
 */
class GameRuleValue {
public:
	GameRuleValue() : m_Value(false) {}

	explicit GameRuleValue(bool value)
		: m_Value(value) {}

	explicit GameRuleValue(int32_t value)
		: m_Value(value) {}

	bool IsBoolean() const { return std::holds_alternative<bool>(m_Value); }
	bool IsInteger() const { return std::holds_alternative<int32_t>(m_Value); }

	bool AsBoolean() const { return std::get<bool>(m_Value); }
	int32_t AsInteger() const { return std::get<int32_t>(m_Value); }

	void Set(bool value) { m_Value = value; }
	void Set(int32_t value) { m_Value = value; }

	/** Parse from string representation ("true"/"false" or decimal). */
	bool SetFromString(const std::string& text);

	/** String representation suitable for commands and NBT. */
	std::string ToString() const;

	bool operator==(const GameRuleValue&) const = default;

private:
	std::variant<bool, int32_t> m_Value;
};

/**
 * Manages all game rules for a world.
 *
 * Rules are registered with a name, default value, and
 * optional description. The vanilla rule set is bootstrapped
 * at construction. Values can be serialized to/from NBT
 * compound tags for level.dat persistence.
 */
class GameRules {
public:
	struct RuleDefinition {
		std::string Name;
		GameRuleValue DefaultValue;
		std::string Description;
	};

	GameRules();

	/** Register a custom rule. Returns false if the name is taken. */
	bool Register(const std::string& name, GameRuleValue defaultValue,
		const std::string& description = "");

	/** Get a rule value, or the default if not set. */
	GameRuleValue Get(const std::string& name) const;

	/** Get a boolean rule. Returns false if not found or wrong type. */
	bool GetBoolean(const std::string& name) const;

	/** Get an integer rule. Returns 0 if not found or wrong type. */
	int32_t GetInteger(const std::string& name) const;

	/** Set a rule value. Returns false if the rule does not exist. */
	bool Set(const std::string& name, GameRuleValue value);

	/** Set from a string. Returns false on parse failure or unknown rule. */
	bool SetFromString(const std::string& name, const std::string& text);

	/** Check if a rule exists. */
	bool Has(const std::string& name) const;

	/** Reset a rule to its default value. */
	void ResetToDefault(const std::string& name);

	/** Reset all rules to defaults. */
	void ResetAllToDefaults();

	/** Serialize all non-default values to a flat key-value map. */
	std::unordered_map<std::string, std::string> Serialize() const;

	/** Deserialize from a flat key-value map. Unknown keys are ignored. */
	void Deserialize(const std::unordered_map<std::string, std::string>& data);

	/** All registered rule names. */
	std::vector<std::string> RuleNames() const;

	/** Get the definition for a rule. */
	const RuleDefinition* Definition(const std::string& name) const;

private:
	void BootstrapVanillaRules();

	std::unordered_map<std::string, RuleDefinition> m_Definitions;
	std::unordered_map<std::string, GameRuleValue> m_Values;
};

}
