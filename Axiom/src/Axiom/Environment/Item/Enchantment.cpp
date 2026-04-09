#include "Enchantment.h"

#include "Axiom/Environment/Item/ItemStack.h"

#include <algorithm>
#include <charconv>
#include <random>
#include <sstream>
#include <unordered_set>

namespace Axiom {

	// ---- EnchantmentRegistry ------------------------------------------

	bool EnchantmentRegistry::s_Bootstrapped = false;

	EnchantmentRegistry& EnchantmentRegistry::Instance() {
		static EnchantmentRegistry instance;
		return instance;
	}

	void EnchantmentRegistry::Register(const std::string& name, EnchantmentType type) {
		m_Types[name] = std::move(type);
	}

	const EnchantmentType* EnchantmentRegistry::Get(const std::string& name) const {
		auto iterator = m_Types.find(name);
		if (iterator != m_Types.end()) {
			return &iterator->second;
		}
		return nullptr;
	}

	void EnchantmentRegistry::Bootstrap() {
		if (s_Bootstrapped) return;
		s_Bootstrapped = true;

		auto& registry = Instance();

		// Protection enchantments
		registry.Register("minecraft:protection", {"minecraft:protection", 4, false});
		registry.Register("minecraft:fire_protection", {"minecraft:fire_protection", 4, false});
		registry.Register("minecraft:blast_protection", {"minecraft:blast_protection", 4, false});
		registry.Register("minecraft:projectile_protection", {"minecraft:projectile_protection", 4, false});
		registry.Register("minecraft:feather_falling", {"minecraft:feather_falling", 4, false});

		// Weapon enchantments
		registry.Register("minecraft:sharpness", {"minecraft:sharpness", 5, false});
		registry.Register("minecraft:smite", {"minecraft:smite", 5, false});
		registry.Register("minecraft:bane_of_arthropods", {"minecraft:bane_of_arthropods", 5, false});
		registry.Register("minecraft:knockback", {"minecraft:knockback", 2, false});
		registry.Register("minecraft:fire_aspect", {"minecraft:fire_aspect", 2, false});
		registry.Register("minecraft:looting", {"minecraft:looting", 3, false});
		registry.Register("minecraft:sweeping_edge", {"minecraft:sweeping_edge", 3, false});

		// Tool enchantments
		registry.Register("minecraft:efficiency", {"minecraft:efficiency", 5, false});
		registry.Register("minecraft:silk_touch", {"minecraft:silk_touch", 1, false});
		registry.Register("minecraft:fortune", {"minecraft:fortune", 3, false});
		registry.Register("minecraft:unbreaking", {"minecraft:unbreaking", 3, false});
		registry.Register("minecraft:mending", {"minecraft:mending", 1, false});

		// Bow enchantments
		registry.Register("minecraft:power", {"minecraft:power", 5, false});
		registry.Register("minecraft:punch", {"minecraft:punch", 2, false});
		registry.Register("minecraft:flame", {"minecraft:flame", 1, false});
		registry.Register("minecraft:infinity", {"minecraft:infinity", 1, false});

		// Armor enchantments
		registry.Register("minecraft:respiration", {"minecraft:respiration", 3, false});
		registry.Register("minecraft:aqua_affinity", {"minecraft:aqua_affinity", 1, false});
		registry.Register("minecraft:thorns", {"minecraft:thorns", 3, false});
		registry.Register("minecraft:depth_strider", {"minecraft:depth_strider", 3, false});
		registry.Register("minecraft:frost_walker", {"minecraft:frost_walker", 2, false});
		registry.Register("minecraft:soul_speed", {"minecraft:soul_speed", 3, false});
		registry.Register("minecraft:swift_sneak", {"minecraft:swift_sneak", 3, false});

		// Curses
		registry.Register("minecraft:binding_curse", {"minecraft:binding_curse", 1, true});
		registry.Register("minecraft:vanishing_curse", {"minecraft:vanishing_curse", 1, true});
	}

	// ---- EnchantmentHelper --------------------------------------------

	/**
	 * Parses the "minecraft:enchantments" component
	 * value. Format is "name:level,name:level" with
	 * no spaces between entries in the string.
	 */
	std::unordered_map<std::string, int32_t> EnchantmentHelper::GetEnchantments(const ItemStack& stack) {
		std::unordered_map<std::string, int32_t> result;

		std::string raw = stack.GetComponent("minecraft:enchantments");
		if (raw.empty()) {
			return result;
		}

		std::istringstream stream(raw);
		std::string entry;
		while (std::getline(stream, entry, ',')) {
			auto colon = entry.rfind(':');
			if (colon == std::string::npos || colon == 0) {
				continue;
			}

			// The name includes the namespace prefix which
			// itself contains a colon, so we split on the
			// last colon to separate name from level.
			std::string name = entry.substr(0, colon);
			std::string levelText = entry.substr(colon + 1);

			int32_t level = 0;
			auto [pointer, errorCode] = std::from_chars(
				levelText.data(), levelText.data() + levelText.size(), level);
			if (errorCode == std::errc() && pointer == levelText.data() + levelText.size()) {
				result[name] = level;
			}
		}

		return result;
	}

	/**
	 * Two enchantments are incompatible when they
	 * belong to the same exclusion group (e.g. the
	 * different protection types or sharpness group).
	 */
	bool EnchantmentHelper::IsCompatible(const std::string& first, const std::string& second) {
		if (first == second) {
			return false;
		}

		static const std::vector<std::unordered_set<std::string>> exclusionGroups = {
			{"minecraft:protection", "minecraft:fire_protection",
			 "minecraft:blast_protection", "minecraft:projectile_protection"},
			{"minecraft:sharpness", "minecraft:smite", "minecraft:bane_of_arthropods"},
			{"minecraft:depth_strider", "minecraft:frost_walker"},
			{"minecraft:silk_touch", "minecraft:fortune"},
			{"minecraft:mending", "minecraft:infinity"},
		};

		for (const auto& group : exclusionGroups) {
			if (group.contains(first) && group.contains(second)) {
				return false;
			}
		}

		return true;
	}

	/**
	 * Applies protection enchantment formula: each
	 * level of protection reduces damage by a fixed
	 * amount, capped so damage never goes below zero.
	 */
	float EnchantmentHelper::ModifyDamageProtection(float damage, const ItemStack& armor) {
		auto enchantments = GetEnchantments(armor);

		int32_t protectionLevel = 0;
		auto iterator = enchantments.find("minecraft:protection");
		if (iterator != enchantments.end()) {
			protectionLevel = iterator->second;
		}

		if (protectionLevel <= 0) {
			return damage;
		}

		// Each level of protection reduces damage by 4%
		float reduction = static_cast<float>(protectionLevel) * 0.04f;
		reduction = std::min(reduction, 0.80f);
		return damage * (1.0f - reduction);
	}

	/**
	 * Unbreaking has a chance to negate durability
	 * loss. For tools the chance is 1/(level+1), so
	 * unbreaking III negates 75% of durability hits.
	 */
	int32_t EnchantmentHelper::ProcessDurabilityChange(int32_t damage, const ItemStack& tool) {
		auto enchantments = GetEnchantments(tool);

		int32_t unbreakingLevel = 0;
		auto iterator = enchantments.find("minecraft:unbreaking");
		if (iterator != enchantments.end()) {
			unbreakingLevel = iterator->second;
		}

		if (unbreakingLevel <= 0) {
			return damage;
		}

		// Each point of damage has a 1/(level+1) chance
		// of actually being applied to the item.
		static thread_local std::mt19937 generator{std::random_device{}()};
		std::uniform_int_distribution<int32_t> distribution(0, unbreakingLevel);

		int32_t actualDamage = 0;
		for (int32_t index = 0; index < damage; ++index) {
			if (distribution(generator) == 0) {
				++actualDamage;
			}
		}

		return actualDamage;
	}

}
