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

		void Read(NetworkBuffer& buffer) override {
			const int32_t length = buffer.ReadInt();
			m_Value.resize(length);
			for (int32_t index = 0; index < length; index++) {
				m_Value[index] = buffer.ReadInt();
			}
		}

		Ref<NbtTag> Clone() const override {
			return CreateRef<NbtIntArray>(m_Value);
		}

		std::string ToString() const override {
			return "[" + std::to_string(m_Value.size()) + " ints]";
		}

	private:
		std::vector<int32_t> m_Value;
	};

}
