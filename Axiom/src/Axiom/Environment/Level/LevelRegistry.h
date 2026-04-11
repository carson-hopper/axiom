#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Core/Tickable.h"
#include "Axiom/Environment/Level/Dimension.h"
#include "Axiom/Environment/Level/DimensionType.h"
#include "Axiom/Environment/Level/Level.h"
#include "Axiom/Environment/Level/Generator/ChunkGenerator.h"

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace Axiom {

	class Dimension;
	class Level;
	class ChunkGenerator;

	/**
	 * Manages all dimensions and their associated levels in the server.
	 * 
	 * The LevelRegistry is responsible for:
	 * - Creating and registering dimensions
	 * - Managing the three vanilla dimensions (Overworld, Nether, End)
	 * - Allowing plugins to register custom dimensions
	 * - Providing dimension lookup by name or type
	 * - Ticking all registered dimensions
	 * 
	 * This class is thread-safe for registration and lookup operations.
	 * Dimension ticking is performed synchronously on the main thread.
	 */
	class LevelRegistry : public Tickable {
	public:
		LevelRegistry();
		~LevelRegistry() override = default;

		/**
		 * Initializes the three vanilla dimensions (Overworld, Nether, End).
		 * Should be called during server startup.
		 */
		void InitializeVanillaDimensions();

		/**
		 * Registers a new dimension with the given properties.
		 * 
		 * @param name Unique name for this dimension (e.g., "minecraft:overworld")
		 * @param type The dimension type defining properties
		 * @param generator The chunk generator for this dimension's level
		 * @return The registered dimension, or nullptr if name already exists
		 */
		Ref<Dimension> RegisterDimension(
			const std::string& name,
			const DimensionType& type,
			Scope<ChunkGenerator> generator
		);

		/**
		 * Unregisters a dimension by name.
		 * This will destroy the dimension and its level.
		 * 
		 * @param name The dimension name to unregister
		 * @return true if the dimension was found and removed
		 */
		bool UnregisterDimension(const std::string& name);

		/**
		 * Gets a dimension by name.
		 * 
		 * @param name The dimension name (e.g., "minecraft:overworld")
		 * @return The dimension, or nullptr if not found
		 */
		Ref<Dimension> GetDimension(const std::string& name);

		/**
		 * Gets the Overworld dimension.
		 */
		Ref<Dimension> GetOverworld();

		/**
		 * Gets the Nether dimension.
		 */
		Ref<Dimension> GetNether();

		/**
		 * Gets the End dimension.
		 */
		Ref<Dimension> GetEnd();

		/**
		 * Gets all registered dimensions.
		 */
		std::vector<Ref<Dimension>> GetAllDimensions();

		/**
		 * Gets the default dimension (Overworld).
		 */
		Ref<Dimension> GetDefaultDimension() { return GetOverworld(); }

		/**
		 * Checks if a dimension with the given name exists.
		 */
		bool HasDimension(const std::string& name);

		/**
		 * Gets the number of registered dimensions.
		 */
		size_t GetDimensionCount() const;

		/**
		 * Called every game tick to tick all dimensions.
		 */
		void OnTick(Timestep timestep) override;

		/**
		 * Gets the tick phase for the registry.
		 * Dimensions tick early to ensure world state is updated before other systems.
		 */
		uint32_t GetTickPhase() const override { return 50; }

	private:
		mutable std::mutex m_Mutex;
		std::unordered_map<std::string, Ref<Dimension>> m_Dimensions;
		
		// Cache for vanilla dimensions
		Ref<Dimension> m_Overworld;
		Ref<Dimension> m_Nether;
		Ref<Dimension> m_End;
	};

}
