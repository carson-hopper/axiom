#pragma once

#include "Axiom/Data/Nbt/NbtTag.h"
#include "Axiom/Network/NetworkBuffer.h"

namespace Axiom {

	class NbtLong : public NbtTag {
	public:
		NbtLong() = default;
		explicit NbtLong(const int64_t value) : m_Value(value) {}

		NbtTagType Type() const override { return NbtTagType::Long; }

		int64_t Value() const { return m_Value; }
		void SetValue(const int64_t value) { m_Value = value; }

		void Write(NetworkBuffer& buffer) const override {
			buffer.WriteLong(m_Value);
		}

		void Read(NetworkBuffer& buffer) override {
			m_Value = buffer.ReadLong();
		}

		Ref<NbtTag> Clone() const override {
			return CreateRef<NbtLong>(m_Value);
		}

		std::string ToString() const override {
			return std::to_string(m_Value) + "L";
		}

	private:
		int64_t m_Value = 0;
	};

}
