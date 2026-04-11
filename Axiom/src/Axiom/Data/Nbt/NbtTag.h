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

	class NbtTag : public Net::NetworkType<void>, public virtual RefCounted {
	public:
		~NbtTag() override = default;

		virtual NbtTagType Type() const = 0;

		void Write(NetworkBuffer& buffer) const override = 0;

		// Trampoline for callers that don't plumb an accounter
		// through; creates a fresh one with the default budget.
		// `final` so subclasses can't silently bypass accounting.
		void Read(NetworkBuffer& buffer) final {
			NbtAccounter accounter;
			Read(buffer, accounter);
		}

		virtual void Read(NetworkBuffer& buffer, NbtAccounter& accounter) = 0;

		virtual Ref<NbtTag> Clone() const = 0;
		virtual std::string ToString() const = 0;
	};

	Ref<NbtTag> CreateNbtTag(NbtTagType type);

}
