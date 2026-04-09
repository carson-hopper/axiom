#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Environment/Block/BlockState.h"
#include "Axiom/Environment/Block/Property.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace Axiom {

	/**
	 * Builds and owns every valid BlockState for a
	 * single block type by computing the Cartesian
	 * product of its registered properties.
	 */
	class StateDefinition {
	public:
		explicit StateDefinition(int32_t blockId);

		/**
		 * Add a property to this
		 * definition.
		 */
		void AddProperty(Ref<BlockProperty> property);

		/**
		 * Generate all states starting from
		 * the given global ID. Returns the
		 * count of states generated.
		 */
		int32_t GenerateStates(int32_t startingGlobalId);

		/**
		 * Get the default state
		 * (first generated).
		 */
		BlockState* DefaultState();

		/**
		 * Look up state by property
		 * values map.
		 */
		BlockState* FindState(const std::unordered_map<std::string, int>& propertyValues);

		/**
		 * Get state by
		 * global ID.
		 */
		BlockState* GetByGlobalId(int32_t globalId);

		const std::vector<Scope<BlockState>>& States() const { return m_States; }
		int32_t StateCount() const { return static_cast<int32_t>(m_States.size()); }

	private:
		int32_t m_BlockId;
		std::vector<Ref<BlockProperty>> m_Properties;
		std::vector<Scope<BlockState>> m_States;
	};

}
