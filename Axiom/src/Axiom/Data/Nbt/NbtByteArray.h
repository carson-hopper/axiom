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

		void Read(NetworkBuffer& buffer, NbtAccounter& accounter) override {
			const int32_t length = buffer.ReadInt();
			if (length < 0) {
				throw std::runtime_error("NbtByteArray: negative length");
			}
			const size_t count = static_cast<size_t>(length);
			if (!accounter.AccountBytes(count)) {
				throw std::runtime_error(accounter.LastError());
			}
			m_Value.resize(count);
			for (size_t index = 0; index < count; index++) {
				m_Value[index] = static_cast<int8_t>(buffer.ReadByte());
			}
		}

		Ref<NbtTag> Clone() const override {
			return Ref<NbtByteArray>::Create(m_Value);
		}

		std::string ToString() const override {
			return "[" + std::to_string(m_Value.size()) + " bytes]";
		}

	private:
		std::vector<int8_t> m_Value;
	};

}
