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

		void Read(NetworkBuffer& buffer, NbtAccounter& accounter) override {
			const int32_t length = buffer.ReadInt();
			if (length < 0) {
				throw std::runtime_error("NbtLongArray: negative length");
			}
			const size_t count = static_cast<size_t>(length);
			if (count > SIZE_MAX / sizeof(int64_t)) {
				throw std::runtime_error("NbtLongArray: length overflow");
			}
			if (!accounter.AccountBytes(count * sizeof(int64_t))) {
				throw std::runtime_error(accounter.LastError());
			}
			m_Value.resize(count);
			for (size_t index = 0; index < count; index++) {
				m_Value[index] = buffer.ReadLong();
			}
		}

		Ref<NbtTag> Clone() const override {
			return Ref<NbtLongArray>::Create(m_Value);
		}

		std::string ToString() const override {
			return "[" + std::to_string(m_Value.size()) + " longs]";
		}

	private:
		std::vector<int64_t> m_Value;
	};

}
