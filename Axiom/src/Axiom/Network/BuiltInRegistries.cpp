#include "BuiltInRegistries.h"

#include "Axiom/Core/Log.h"

#include <stdexcept>

namespace Axiom {

	bool BuiltInRegistries::s_Bootstrapped = false;

	// ----- Static registry instances --------------------------------

	static Registry<std::string>& BlocksRegistry() {
		static Registry<std::string> registry(ResourceLocation("minecraft", "block"));
		return registry;
	}

	static Registry<std::string>& ItemsRegistry() {
		static Registry<std::string> registry(ResourceLocation("minecraft", "item"));
		return registry;
	}

	static Registry<std::string>& BiomesRegistry() {
		static Registry<std::string> registry(ResourceLocation("minecraft", "worldgen/biome"));
		return registry;
	}

	static Registry<std::string>& DimensionsRegistry() {
		static Registry<std::string> registry(ResourceLocation("minecraft", "dimension_type"));
		return registry;
	}

	static Registry<std::string>& EntityTypesRegistry() {
		static Registry<std::string> registry(ResourceLocation("minecraft", "entity_type"));
		return registry;
	}

	static Registry<std::string>& BlockEntityTypesRegistry() {
		static Registry<std::string> registry(ResourceLocation("minecraft", "block_entity_type"));
		return registry;
	}

	static Registry<std::string>& EnchantmentsRegistry() {
		static Registry<std::string> registry(ResourceLocation("minecraft", "enchantment"));
		return registry;
	}

	static Registry<std::string>& MobEffectsRegistry() {
		static Registry<std::string> registry(ResourceLocation("minecraft", "mob_effect"));
		return registry;
	}

	static Registry<std::string>& PotionTypesRegistry() {
		static Registry<std::string> registry(ResourceLocation("minecraft", "potion"));
		return registry;
	}

	static Registry<std::string>& ParticleTypesRegistry() {
		static Registry<std::string> registry(ResourceLocation("minecraft", "particle_type"));
		return registry;
	}

	static Registry<std::string>& SoundEventsRegistry() {
		static Registry<std::string> registry(ResourceLocation("minecraft", "sound_event"));
		return registry;
	}

	static Registry<std::string>& GameEventsRegistry() {
		static Registry<std::string> registry(ResourceLocation("minecraft", "game_event"));
		return registry;
	}

	static Registry<std::string>& RecipeTypesRegistry() {
		static Registry<std::string> registry(ResourceLocation("minecraft", "recipe_type"));
		return registry;
	}

	// ----- Helper to register a minecraft-namespaced entry ----------

	static void RegisterMinecraft(Registry<std::string>& registry, const std::string& path) {
		ResourceLocation key("minecraft", path);
		registry.Register(key, path);
	}

	// ----- Bootstrap ------------------------------------------------

	void BuiltInRegistries::Bootstrap() {
		if (s_Bootstrapped) {
			throw std::runtime_error("BuiltInRegistries already bootstrapped");
		}

		// Core blocks
		RegisterMinecraft(BlocksRegistry(), "air");
		RegisterMinecraft(BlocksRegistry(), "stone");
		RegisterMinecraft(BlocksRegistry(), "dirt");
		RegisterMinecraft(BlocksRegistry(), "grass_block");
		RegisterMinecraft(BlocksRegistry(), "bedrock");
		RegisterMinecraft(BlocksRegistry(), "water");
		RegisterMinecraft(BlocksRegistry(), "lava");

		// Core items
		RegisterMinecraft(ItemsRegistry(), "air");
		RegisterMinecraft(ItemsRegistry(), "stone");
		RegisterMinecraft(ItemsRegistry(), "dirt");

		// Dimensions
		RegisterMinecraft(DimensionsRegistry(), "overworld");
		RegisterMinecraft(DimensionsRegistry(), "the_nether");
		RegisterMinecraft(DimensionsRegistry(), "the_end");

		// Biomes
		RegisterMinecraft(BiomesRegistry(), "plains");
		RegisterMinecraft(BiomesRegistry(), "desert");
		RegisterMinecraft(BiomesRegistry(), "ocean");
		RegisterMinecraft(BiomesRegistry(), "forest");

		s_Bootstrapped = true;
		AX_CORE_INFO("Built-in registries bootstrapped ({} blocks, {} items, {} dimensions, {} biomes)",
			BlocksRegistry().Size(), ItemsRegistry().Size(),
			DimensionsRegistry().Size(), BiomesRegistry().Size());
	}

	bool BuiltInRegistries::IsBootstrapped() {
		return s_Bootstrapped;
	}

	// ----- Accessors ------------------------------------------------

	Registry<std::string>& BuiltInRegistries::Blocks() { return BlocksRegistry(); }
	Registry<std::string>& BuiltInRegistries::Items() { return ItemsRegistry(); }
	Registry<std::string>& BuiltInRegistries::Biomes() { return BiomesRegistry(); }
	Registry<std::string>& BuiltInRegistries::Dimensions() { return DimensionsRegistry(); }
	Registry<std::string>& BuiltInRegistries::EntityTypes() { return EntityTypesRegistry(); }
	Registry<std::string>& BuiltInRegistries::BlockEntityTypes() { return BlockEntityTypesRegistry(); }
	Registry<std::string>& BuiltInRegistries::Enchantments() { return EnchantmentsRegistry(); }
	Registry<std::string>& BuiltInRegistries::MobEffects() { return MobEffectsRegistry(); }
	Registry<std::string>& BuiltInRegistries::PotionTypes() { return PotionTypesRegistry(); }
	Registry<std::string>& BuiltInRegistries::ParticleTypes() { return ParticleTypesRegistry(); }
	Registry<std::string>& BuiltInRegistries::SoundEvents() { return SoundEventsRegistry(); }
	Registry<std::string>& BuiltInRegistries::GameEvents() { return GameEventsRegistry(); }
	Registry<std::string>& BuiltInRegistries::RecipeTypes() { return RecipeTypesRegistry(); }

}
