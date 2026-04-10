#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Network/NetworkBuffer.h"

#include <cstdint>

namespace Axiom {

	/**
	 * Base class for Brigadier argument parsers. Each parser has a numeric
	 * ID from the vanilla `minecraft:command_argument_type` registry and
	 * optional properties that describe its constraints (min/max, flags,
	 * registry identifier, etc.).
	 *
	 * Parsers are attached to ArgumentNodes to tell the client what kind
	 * of value it should expect and how to render the autocomplete hint.
	 */
	class ArgumentParser : public virtual RefCounted {
	public:
		~ArgumentParser() override = default;

		/** Index into the command_argument_type registry. */
		virtual int32_t ParserId() const = 0;

		/** Write any parser-specific properties after the ID. */
		virtual void WriteProperties(NetworkBuffer& /*buffer*/) const {}
	};

}
