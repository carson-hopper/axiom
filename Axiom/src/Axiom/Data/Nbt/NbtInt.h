#pragma once

#include "Axiom/Data/Nbt/NbtTag.h"
#include "Axiom/Network/NetworkBuffer.h"

namespace Axiom {

	class NbtInt : public NbtTag {
	public:
		NbtInt() = default;
		explicit NbtInt(const int32_t value) : m_Value(value) {}

		NbtTagType Type() const override { return NbtTagType::Int; }

		int32_t Value() const { return m_Value; }
		void SetValue(const int32_t value) { m_Value = value; }

		void Write(NetworkBuffer& buffer) const override {
			buffer.WriteInt(m_Value);
		}

		void Read(NetworkBuffer& buffer) override {
			m_Value = buffer.ReadInt();
		}

		Ref<NbtTag> Clone() const override {
			return CreateRef<NbtInt>(m_Value);
		}

		std::string ToString() const override {
			return std::to_string(m_Value);
		}

	private:
		int32_t m_Value = 0;
	};

}
