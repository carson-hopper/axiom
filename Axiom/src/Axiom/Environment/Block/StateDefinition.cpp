#include "StateDefinition.h"

namespace Axiom {

	StateDefinition::StateDefinition(int32_t blockId)
		: m_BlockId(blockId) {}

	void StateDefinition::AddProperty(Ref<BlockProperty> property) {
		m_Properties.push_back(std::move(property));
	}

	int32_t StateDefinition::GenerateStates(int32_t startingGlobalId) {
		m_States.clear();

		if (m_Properties.empty()) {
			m_States.push_back(CreateScope<BlockState>(startingGlobalId, m_BlockId));
			return 1;
		}

		// Compute total state count as the product of all
		// property value counts (Cartesian product size).
		int32_t totalStates = 1;
		for (const auto& property : m_Properties) {
			totalStates *= property->ValueCount();
		}

		// Generate each state by decomposing a flat index
		// into per-property indices via repeated division.
		for (int32_t stateIndex = 0; stateIndex < totalStates; ++stateIndex) {
			auto state = CreateScope<BlockState>(startingGlobalId + stateIndex, m_BlockId);

			int32_t remaining = stateIndex;
			for (int propertyIndex = static_cast<int>(m_Properties.size()) - 1; propertyIndex >= 0; --propertyIndex) {
				const auto& property = m_Properties[static_cast<size_t>(propertyIndex)];
				int valueCount = property->ValueCount();
				int valueIndex = remaining % valueCount;
				remaining /= valueCount;
				state->SetPropertyValue(property->Name(), valueIndex);
			}

			m_States.push_back(std::move(state));
		}

		return totalStates;
	}

	BlockState* StateDefinition::DefaultState() {
		if (m_States.empty()) return nullptr;
		return m_States[0].get();
	}

	BlockState* StateDefinition::FindState(const std::unordered_map<std::string, int>& propertyValues) {
		for (const auto& state : m_States) {
			bool match = true;
			for (const auto& [name, value] : propertyValues) {
				if (state->GetPropertyValue(name, -1) != value) {
					match = false;
					break;
				}
			}
			if (match) return state.get();
		}
		return nullptr;
	}

	BlockState* StateDefinition::GetByGlobalId(int32_t globalId) {
		for (const auto& state : m_States) {
			if (state->GlobalId() == globalId) {
				return state.get();
			}
		}
		return nullptr;
	}

}
