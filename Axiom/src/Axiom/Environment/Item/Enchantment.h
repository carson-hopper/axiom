#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

namespace Axiom {

	class ItemStack;

	/**
	 * Describes a single enchantment type
	 * with its name, max level, and whether
	 * it is classified as a curse.
	 */
	struct EnchantmentType {
		std::string Name;
		int32_t MaxLevel = 1;
		bool IsCurse = false;
	};

	/**
	 * Global registry of all known enchantment
	 * types. Call Bootstrap() once at startup to
	 * populate the vanilla enchantment set.
	 */
	class EnchantmentRegistry {
	public:
		static EnchantmentRegistry& Instance();

		void Register(const std::string& name, EnchantmentType type);
		const EnchantmentType* Get(const std::string& name) const;

		/** Register vanilla enchantment types. */
		static void Bootstrap();

	private:
		std::unordered_map<std::string, EnchantmentType> m_Types;
		static bool s_Bootstrapped;
	};

	/**
	 * Utility functions for reading and
	 * applying enchantment effects from
	 * item stack component data.
	 */
	class EnchantmentHelper {
	public:
		/**
		 * Get enchantments from an
		 * ItemStack's component data.
		 */
		static std::unordered_map<std::string, int32_t> GetEnchantments(const ItemStack& stack);

		/**
		 * Check if two enchantments
		 * are compatible with each other.
		 */
		static bool IsCompatible(const std::string& first, const std::string& second);

		/**
		 * Calculate damage protection
		 * from armor enchantments.
		 */
		static float ModifyDamageProtection(float damage, const ItemStack& armor);

		/**
		 * Process durability change based
		 * on unbreaking enchantment level.
		 */
		static int32_t ProcessDurabilityChange(int32_t damage, const ItemStack& tool);
	};

}
