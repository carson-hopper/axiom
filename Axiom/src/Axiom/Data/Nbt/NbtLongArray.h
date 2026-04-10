#pragma once

#include "Axiom/Data/Nbt/NbtTag.h"
#include "Axiom/Network/NetworkBuffer.h"

#include <utility>
#include <vector>

namespace Axiom {

	class NbtLongArray : public NbtTag {
	public:
		NbtLongArray() = default;
		explicit NbtLongArray(std::vector<int64_t> value) : m_Value(std::move(value)) {}

		NbtTagType Type() const override { return NbtTagType::LongArray; }

		const std::vector<int64_t>& Value() const { return m_Value; }
		std::vector<int64_t>& Value() { return m_Value; }
		void SetValue(std::vector<int64_t> value) { m_Value = std::move(value); }

		void Write(NetworkBuffer& buffer) const override {
			buffer.WriteInt(static_cast<int32_t>(m_Value.size()));
			for (const int64_t longValue : m_Value) {
				buffer.WriteLong(longValue);
			}
		}

		void Read(NetworkBuffer& buffer) override {
			const int32_t length = buffer.ReadInt();
			m_Value.resize(length);
			for (int32_t index = 0; index < length; index++) {
				m_Value[index] = buffer.ReadLong();
			}
		}

		Ref<NbtTag> Clone() const override {
			return CreateRef<NbtLongArray>(m_Value);
		}

		std::string ToString() const override {
			return "[" + std::to_string(m_Value.size()) + " longs]";
		}

	private:
		std::vector<int64_t> m_Value;
	};

}
