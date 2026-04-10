#pragma once

#include "Axiom/Data/Nbt/NbtTag.h"
#include "Axiom/Network/NetworkBuffer.h"

namespace Axiom {

	class NbtShort : public NbtTag {
	public:
		NbtShort() = default;
		explicit NbtShort(const int16_t value) : m_Value(value) {}

		NbtTagType Type() const override { return NbtTagType::Short; }

		int16_t Value() const { return m_Value; }
		void SetValue(const int16_t value) { m_Value = value; }

		void Write(NetworkBuffer& buffer) const override {
			buffer.WriteShort(m_Value);
		}

		void Read(NetworkBuffer& buffer) override {
			m_Value = buffer.ReadShort();
		}

		Ref<NbtTag> Clone() const override {
			return CreateRef<NbtShort>(m_Value);
		}

		std::string ToString() const override {
			return std::to_string(m_Value) + "s";
		}

	private:
		int16_t m_Value = 0;
	};

}
