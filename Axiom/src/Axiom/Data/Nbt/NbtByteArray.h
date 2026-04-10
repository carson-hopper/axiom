#pragma once

#include "Axiom/Data/Nbt/NbtTag.h"
#include "Axiom/Network/NetworkBuffer.h"

#include <utility>
#include <vector>

namespace Axiom {

	class NbtByteArray : public NbtTag {
	public:
		NbtByteArray() = default;
		explicit NbtByteArray(std::vector<int8_t> value) : m_Value(std::move(value)) {}

		NbtTagType Type() const override { return NbtTagType::ByteArray; }

		const std::vector<int8_t>& Value() const { return m_Value; }
		std::vector<int8_t>& Value() { return m_Value; }
		void SetValue(std::vector<int8_t> value) { m_Value = std::move(value); }

		void Write(NetworkBuffer& buffer) const override {
			buffer.WriteInt(static_cast<int32_t>(m_Value.size()));
			for (const int8_t byte : m_Value) {
				buffer.WriteByte(static_cast<uint8_t>(byte));
			}
		}

		void Read(NetworkBuffer& buffer) override {
			const int32_t length = buffer.ReadInt();
			m_Value.resize(length);
			for (int32_t index = 0; index < length; index++) {
				m_Value[index] = static_cast<int8_t>(buffer.ReadByte());
			}
		}

		Ref<NbtTag> Clone() const override {
			return CreateRef<NbtByteArray>(m_Value);
		}

		std::string ToString() const override {
			return "[" + std::to_string(m_Value.size()) + " bytes]";
		}

	private:
		std::vector<int8_t> m_Value;
	};

}
