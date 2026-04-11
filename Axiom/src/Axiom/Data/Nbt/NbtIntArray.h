#pragma once

#include "Axiom/Data/Nbt/NbtTag.h"
#include "Axiom/Network/NetworkBuffer.h"

#include <utility>
#include <vector>

namespace Axiom {

	class NbtIntArray : public NbtTag {
	public:
		NbtIntArray() = default;
		explicit NbtIntArray(std::vector<int32_t> value) : m_Value(std::move(value)) {}

		NbtTagType Type() const override { return NbtTagType::IntArray; }

		const std::vector<int32_t>& Value() const { return m_Value; }
		std::vector<int32_t>& Value() { return m_Value; }
		void SetValue(std::vector<int32_t> value) { m_Value = std::move(value); }

		void Write(NetworkBuffer& buffer) const override {
			buffer.WriteInt(static_cast<int32_t>(m_Value.size()));
			for (const int32_t integer : m_Value) {
				buffer.WriteInt(integer);
			}
		}

		void Read(NetworkBuffer& buffer, NbtAccounter& accounter) override {
			const int32_t length = buffer.ReadInt();
			if (length < 0) {
				throw std::runtime_error("NbtIntArray: negative length");
			}
			const size_t count = static_cast<size_t>(length);
			if (count > SIZE_MAX / sizeof(int32_t)) {
				throw std::runtime_error("NbtIntArray: length overflow");
			}
			if (!accounter.AccountBytes(count * sizeof(int32_t))) {
				throw std::runtime_error(accounter.LastError());
			}
			m_Value.resize(count);
			for (size_t index = 0; index < count; index++) {
				m_Value[index] = buffer.ReadInt();
			}
		}

		Ref<NbtTag> Clone() const override {
			return Ref<NbtIntArray>::Create(m_Value);
		}

		std::string ToString() const override {
			return "[" + std::to_string(m_Value.size()) + " ints]";
		}

	private:
		std::vector<int32_t> m_Value;
	};

}
