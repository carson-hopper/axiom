#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Network/Type/NetworkType.h"

#include <cstdint>
#include <string>
#include <vector>

namespace Axiom {

	class NetworkBuffer;

	enum class NbtTagType : uint8_t {
		End = 0,
		Byte = 1,
		Short = 2,
		Int = 3,
		Long = 4,
		Float = 5,
		Double = 6,
		ByteArray = 7,
		String = 8,
		List = 9,
		Compound = 10,
		IntArray = 11,
		LongArray = 12,
	};

	/**
	 * Base class for all NBT tag types. Inherits from Net::NetworkType<void>,
	 * the polymorphic specialization for types that serialize themselves.
	 * Subclasses override Read / Write to (de)serialize their payload bytes.
	 * Root framing (type byte, name) is handled by NbtIo for file I/O and
	 * by the Packet dispatch for network fields.
	 */
	class NbtTag : public Net::NetworkType<void>, public virtual RefCounted {
	public:
		~NbtTag() override = default;

		virtual NbtTagType Type() const = 0;

		/**
		 * Write the tag's payload bytes (no type byte, no name).
		 * Subclasses override this to encode their data.
		 */
		void Write(NetworkBuffer& buffer) const override = 0;

		/**
		 * Read the tag's payload bytes (caller has already consumed any
		 * framing). Subclasses override this to decode their data.
		 */
		void Read(NetworkBuffer& buffer) override = 0;

		/** Create a deep copy of this tag. */
		virtual Ref<NbtTag> Clone() const = 0;

		/** Human-readable description for debugging. */
		virtual std::string ToString() const = 0;
	};

	/**
	 * Create a default-constructed tag for the given type. Used by readers
	 * to materialize children while parsing compounds and lists.
	 */
	Ref<NbtTag> CreateNbtTag(NbtTagType type);

}
