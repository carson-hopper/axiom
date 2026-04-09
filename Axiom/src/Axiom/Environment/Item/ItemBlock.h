#pragma once

#include "Axiom/Environment/Item/Item.h"
#include "Axiom/Environment/Block/Block.h"

namespace Axiom {

	/**
	 * An item that places a block when used.
	 * Most blocks have a corresponding ItemBlock (stone item places stone block).
	 */
	class ItemBlock : public Item {
	public:
		ItemBlock(const int32_t itemId, std::string name, Ref<Block> block)
			: Item(itemId, std::move(name))
			, m_Block(std::move(block)) {}

		Ref<Block> GetBlock() const { return m_Block; }

	private:
		Ref<Block> m_Block;
	};

}
