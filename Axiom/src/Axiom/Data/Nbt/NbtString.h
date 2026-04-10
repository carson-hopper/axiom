#pragma once

#include "Axiom/Data/Nbt/NbtTag.h"
#include "Axiom/Network/NetworkBuffer.h"

#include <utility>

namespace Axiom {

	class NbtString : public NbtTag {
	public:
		NbtString() = default;
		explicit NbtString(std::string value) : m_Value(std::move(value)) {}

		NbtTagType Type() const override { return NbtTagType::String; }

		const std::string& Value() const { return m_Value; }
		void SetValue(std::string value) { m_Value = std::move(value); }

		void Write(NetworkBuffer& buffer) const override {
			buffer.WriteUnsignedShort(static_cast<uint16_t>(m_Value.size()));
			for (const char character : m_Value) {
				buffer.WriteByte(static_cast<uint8_t>(character));
			}
		}

		void Read(NetworkBuffer& buffer) override {
			const uint16_t length = static_cast<uint16_t>(buffer.ReadShort()) & 0xFFFFU;
			m_Value.resize(length);
			for (uint16_t index = 0; index < length; index++) {
				m_Value[index] = static_cast<char>(buffer.ReadByte());
			}
		}

		Ref<NbtTag> Clone() const override {
			return CreateRef<NbtString>(m_Value);
		}

		std::string ToString() const override {
			return "\"" + m_Value + "\"";
		}

	private:
		std::string m_Value;
	};

}
