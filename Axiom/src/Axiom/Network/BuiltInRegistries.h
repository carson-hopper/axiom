#pragma once

#include "Axiom/Network/Registry.h"

#include <string>

namespace Axiom {

	/**
	 * Bootstraps the minimum set of registries
	 * needed for the server to operate.
	 * Each registry stores string
	 * stubs for now.
	 */
	class BuiltInRegistries {
	public:
		static void Bootstrap();
		static bool IsBootstrapped();

		static Registry<std::string>& Blocks();
		static Registry<std::string>& Items();
		static Registry<std::string>& Biomes();
		static Registry<std::string>& Dimensions();
		static Registry<std::string>& EntityTypes();
		static Registry<std::string>& BlockEntityTypes();
		static Registry<std::string>& Enchantments();
		static Registry<std::string>& MobEffects();
		static Registry<std::string>& PotionTypes();
		static Registry<std::string>& ParticleTypes();
		static Registry<std::string>& SoundEvents();
		static Registry<std::string>& GameEvents();
		static Registry<std::string>& RecipeTypes();

	private:
		static bool s_Bootstrapped;
	};

}
