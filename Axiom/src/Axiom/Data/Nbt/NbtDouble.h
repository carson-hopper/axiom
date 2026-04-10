#pragma once

#include "Axiom/Data/Nbt/NbtTag.h"
#include "Axiom/Network/NetworkBuffer.h"

namespace Axiom {

	class NbtDouble : public NbtTag {
	public:
		NbtDouble() = default;
		explicit NbtDouble(const double value) : m_Value(value) {}

		NbtTagType Type() const override { return NbtTagType::Double; }

		double Value() const { return m_Value; }
		void SetValue(const double value) { m_Value = value; }

		void Write(NetworkBuffer& buffer) const override {
			buffer.WriteDouble(m_Value);
		}

		void Read(NetworkBuffer& buffer) override {
			m_Value = buffer.ReadDouble();
		}

		Ref<NbtTag> Clone() const override {
			return CreateRef<NbtDouble>(m_Value);
		}

		std::string ToString() const override {
			return std::to_string(m_Value) + "d";
		}

	private:
		double m_Value = 0.0;
	};

}
