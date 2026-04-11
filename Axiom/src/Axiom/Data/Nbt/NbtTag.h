#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Data/Nbt/NbtAccounter.h"
#include "Axiom/Network/Type/NetworkType.h"

#include <cstdint>
#include <stdexcept>
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
	 * Base class for all NBT tag types. Inherits from
	 * Net::NetworkType<void>, the polymorphic specialization
	 * for types that serialize themselves. Subclasses
	 * override Write / Read to (de)serialize payload bytes.
	 * Root framing (type byte, name) is handled by NbtIo
	 * for file I/O and by the Packet dispatch for network
	 * fields.
	 *
	 * Every read must pass through an `NbtAccounter` —
	 * subclasses override the two-argument `Read` overload
	 * and account every byte they consume. The one-argument
	 * trampoline below exists for callers that don't want
	 * to plumb an accounter through (it creates a fresh
	 * one with the default 2 MiB budget per call) and
	 * satisfies the `NetworkType<void>` pure-virtual.
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
		 * Trampoline that satisfies `NetworkType<void>`'s
		 * no-accounter `Read` override. Creates a fresh
		 * `NbtAccounter` with the default budget and
		 * delegates to the bounds-checked overload.
		 *
		 * Marked `final` so subclasses can't accidentally
		 * override the wrong overload and silently drop
		 * their accounting path.
		 */
		void Read(NetworkBuffer& buffer) final {
			NbtAccounter accounter;
			Read(buffer, accounter);
		}

		/**
		 * Bounds-checked payload read. Each subclass MUST
		 * account every byte it reads through `accounter`
		 * and hold an `NbtAccounter::DepthGuard` around
		 * any recursive reads into nested lists or
		 * compounds. Throws `std::runtime_error` on
		 * budget or depth exhaustion so parsing aborts
		 * cleanly instead of exhausting memory or the
		 * call stack.
		 */
		virtual void Read(NetworkBuffer& buffer, NbtAccounter& accounter) = 0;

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
