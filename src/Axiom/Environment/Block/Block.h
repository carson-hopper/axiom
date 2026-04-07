#pragma once

#include "Axiom/Core/Base.h"

#include <cstdint>
#include <string>

namespace Axiom {

	/**
	 * Represents a block type in the game.
	 * A Block defines the properties of a type of block (stone, dirt, etc.)
	 * It is NOT an instance in the world — block state IDs represent placed blocks.
	 */
	class Block {
	public:
		Block(const int32_t blockId, std::string name, const int32_t defaultStateId)
			: m_BlockId(blockId)
			, m_Name(std::move(name))
			, m_DefaultStateId(defaultStateId) {}

		virtual ~Block() = default;

		int32_t BlockId() const { return m_BlockId; }
		const std::string& Name() const { return m_Name; }
		int32_t DefaultStateId() const { return m_DefaultStateId; }

		float Hardness() const { return m_Hardness; }
		void SetHardness(float hardness) { m_Hardness = hardness; }

		float BlastResistance() const { return m_BlastResistance; }
		void SetBlastResistance(float resistance) { m_BlastResistance = resistance; }

		float Slipperiness() const { return m_Slipperiness; }
		void SetSlipperiness(float slipperiness) { m_Slipperiness = slipperiness; }

		bool IsAir() const { return m_DefaultStateId == 0; }
		bool IsSolid() const { return m_Hardness >= 0.0f && !IsAir(); }

	private:
		int32_t m_BlockId;
		std::string m_Name;
		int32_t m_DefaultStateId;
		float m_Hardness = 0.0f;
		float m_BlastResistance = 0.0f;
		float m_Slipperiness = 0.6f;
	};

}
