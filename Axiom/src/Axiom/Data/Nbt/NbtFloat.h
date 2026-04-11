#pragma once

#include "Axiom/Data/Nbt/NbtTag.h"
#include "Axiom/Network/NetworkBuffer.h"

namespace Axiom {

	class NbtFloat : public NbtTag {
	public:
		NbtFloat() = default;
		explicit NbtFloat(const float value) : m_Value(value) {}

		NbtTagType Type() const override { return NbtTagType::Float; }

		float Value() const { return m_Value; }
		void SetValue(const float value) { m_Value = value; }

		void Write(NetworkBuffer& buffer) const override {
			buffer.WriteFloat(m_Value);
		}

		void Read(NetworkBuffer& buffer, NbtAccounter& accounter) override {
			if (!accounter.AccountBytes(sizeof(float))) {
				throw std::runtime_error(accounter.LastError());
			}
			m_Value = buffer.ReadFloat();
		}

		Ref<NbtTag> Clone() const override {
			return Ref<NbtFloat>::Create(m_Value);
		}

		std::string ToString() const override {
			return std::to_string(m_Value) + "f";
		}

	private:
		float m_Value = 0.0F;
	};

}
