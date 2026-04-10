#pragma once

#include "Axiom/Data/Nbt/NbtTag.h"
#include "Axiom/Network/NetworkBuffer.h"

namespace Axiom {

	class NbtByte : public NbtTag {
	public:
		NbtByte() = default;
		explicit NbtByte(const int8_t value) : m_Value(value) {}

		NbtTagType Type() const override { return NbtTagType::Byte; }

		int8_t Value() const { return m_Value; }
		void SetValue(const int8_t value) { m_Value = value; }

		void Write(NetworkBuffer& buffer) const override {
			buffer.WriteByte(static_cast<uint8_t>(m_Value));
		}

		void Read(NetworkBuffer& buffer) override {
			m_Value = static_cast<int8_t>(buffer.ReadByte());
		}

		Ref<NbtTag> Clone() const override {
			return CreateRef<NbtByte>(m_Value);
		}

		std::string ToString() const override {
			return std::to_string(static_cast<int>(m_Value)) + "b";
		}

	private:
		int8_t m_Value = 0;
	};

}
