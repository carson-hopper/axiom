#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

namespace Axiom {

	/**
	 * Represents a single block state — one specific
	 * combination of property values for a block type.
	 * Each state has a unique global ID used by the
	 * protocol and a block ID linking to its parent.
	 */
	class BlockState {
	public:
		BlockState(int32_t globalId, int32_t blockId);

		int32_t GlobalId() const { return m_GlobalId; }
		int32_t BlockId() const { return m_BlockId; }

		void SetPropertyValue(const std::string& propertyName, int value);
		int GetPropertyValue(const std::string& propertyName, int fallback = 0) const;
		bool HasProperty(const std::string& propertyName) const;

		bool IsAir() const { return m_GlobalId == 0; }
		bool IsSolid() const { return m_Solid; }
		void SetSolid(bool solid) { m_Solid = solid; }

	private:
		int32_t m_GlobalId;
		int32_t m_BlockId;
		bool m_Solid = true;
		std::unordered_map<std::string, int> m_PropertyValues;
	};

}
