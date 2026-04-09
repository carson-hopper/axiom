#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Environment/Block/BlockEntity.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Axiom {

	/**
	 * Registry-backed factory that maps resource
	 * locations (e.g. "minecraft:chest") to
	 * concrete BlockEntity constructors with
	 * valid-block-set validation.
	 */
	class BlockEntityType {
	public:
		using Factory = std::function<Scope<BlockEntity>(int32_t, int32_t, int32_t)>;

		BlockEntityType(std::string identifier, Factory factory, std::unordered_set<std::string> validBlocks)
			: m_Identifier(std::move(identifier))
			, m_Factory(std::move(factory))
			, m_ValidBlocks(std::move(validBlocks)) {}

		/**
		 * Create a new BlockEntity instance
		 * at the given position.
		 */
		Scope<BlockEntity> Create(int32_t positionX, int32_t positionY, int32_t positionZ) const {
			return m_Factory(positionX, positionY, positionZ);
		}

		/**
		 * Check whether a block name is valid
		 * for this block entity type.
		 */
		bool IsValidBlock(const std::string& blockName) const {
			return m_ValidBlocks.empty() || m_ValidBlocks.contains(blockName);
		}

		const std::string& Identifier() const { return m_Identifier; }
		const std::unordered_set<std::string>& ValidBlocks() const { return m_ValidBlocks; }

		// ----- Static registry ------------------------------------------

		/**
		 * Register a block entity type
		 * with the global registry.
		 */
		static void Register(BlockEntityType type);

		/**
		 * Look up a block entity type by
		 * its resource location identifier.
		 */
		static const BlockEntityType* Get(const std::string& identifier);

		/**
		 * Get all registered block
		 * entity types.
		 */
		static const std::unordered_map<std::string, BlockEntityType>& All();

	private:
		std::string m_Identifier;
		Factory m_Factory;
		std::unordered_set<std::string> m_ValidBlocks;

		static std::unordered_map<std::string, BlockEntityType>& Registry();
	};

}
