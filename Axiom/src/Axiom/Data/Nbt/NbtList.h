#pragma once

#include "Axiom/Data/Nbt/NbtTag.h"
#include "Axiom/Network/NetworkBuffer.h"

#include <vector>

namespace Axiom {

	class NbtCompound;

	/**
	 * Homogeneous list of NBT tags. All elements must have the same type.
	 * Adding a tag of the wrong type is silently ignored.
	 */
	class NbtList : public NbtTag {
	public:
		NbtList() = default;
		explicit NbtList(const NbtTagType elementType) : m_ElementType(elementType) {}

		NbtTagType Type() const override { return NbtTagType::List; }

		NbtTagType ElementType() const { return m_ElementType; }
		size_t Size() const { return m_Elements.size(); }
		bool IsEmpty() const { return m_Elements.empty(); }

		void Add(const Ref<NbtTag>& tag) {
			if (!tag) return;
			if (m_Elements.empty() && m_ElementType == NbtTagType::End) {
				m_ElementType = tag->Type();
			}
			if (tag->Type() != m_ElementType) return;
			m_Elements.push_back(tag);
		}

		Ref<NbtTag> Get(const size_t index) const {
			if (index >= m_Elements.size()) return nullptr;
			return m_Elements[index];
		}

		Ref<NbtCompound> GetCompound(size_t index) const;
		Ref<NbtList> GetList(size_t index) const;
		std::string GetString(size_t index) const;
		int32_t GetInt(size_t index) const;
		int64_t GetLong(size_t index) const;
		float GetFloat(size_t index) const;
		double GetDouble(size_t index) const;

		const std::vector<Ref<NbtTag>>& Elements() const { return m_Elements; }

		void Write(NetworkBuffer& buffer) const override {
			buffer.WriteByte(static_cast<uint8_t>(m_ElementType));
			buffer.WriteInt(static_cast<int32_t>(m_Elements.size()));
			for (const auto& element : m_Elements) {
				element->Write(buffer);
			}
		}

		void Read(NetworkBuffer& buffer, NbtAccounter& accounter) override {
			NbtAccounter::DepthGuard depth(accounter);

			m_ElementType = static_cast<NbtTagType>(buffer.ReadByte());
			const int32_t count = buffer.ReadInt();
			if (count < 0) {
				throw std::runtime_error("NbtList: negative count");
			}
			const size_t elementCount = static_cast<size_t>(count);
			// Account for the pointer-array overhead before
			// we `reserve` — a malicious count of 2 billion
			// would otherwise try to allocate ~16 GiB of
			// `Ref<NbtTag>` slots before the first read.
			if (elementCount > SIZE_MAX / sizeof(Ref<NbtTag>)) {
				throw std::runtime_error("NbtList: count overflow");
			}
			if (!accounter.AccountBytes(elementCount * sizeof(Ref<NbtTag>))) {
				throw std::runtime_error(accounter.LastError());
			}

			m_Elements.clear();
			m_Elements.reserve(elementCount);
			for (size_t index = 0; index < elementCount; index++) {
				auto element = CreateNbtTag(m_ElementType);
				if (element) {
					element->Read(buffer, accounter);
					m_Elements.push_back(element);
				}
			}
		}

		Ref<NbtTag> Clone() const override {
			auto copy = Ref<NbtList>::Create(m_ElementType);
			copy->m_Elements.reserve(m_Elements.size());
			for (const auto& element : m_Elements) {
				copy->m_Elements.push_back(element->Clone());
			}
			return copy;
		}

		std::string ToString() const override {
			return "[" + std::to_string(m_Elements.size()) + " entries]";
		}

	private:
		NbtTagType m_ElementType = NbtTagType::End;
		std::vector<Ref<NbtTag>> m_Elements;
	};

}
