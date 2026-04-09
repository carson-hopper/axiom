#include "BlockState.h"

namespace Axiom {

	BlockState::BlockState(int32_t globalId, int32_t blockId)
		: m_GlobalId(globalId)
		, m_BlockId(blockId) {}

	void BlockState::SetPropertyValue(const std::string& propertyName, int value) {
		m_PropertyValues[propertyName] = value;
	}

	int BlockState::GetPropertyValue(const std::string& propertyName, int fallback) const {
		auto iterator = m_PropertyValues.find(propertyName);
		if (iterator != m_PropertyValues.end()) {
			return iterator->second;
		}
		return fallback;
	}

	bool BlockState::HasProperty(const std::string& propertyName) const {
		return m_PropertyValues.contains(propertyName);
	}

}
