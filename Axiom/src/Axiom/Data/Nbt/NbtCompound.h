#pragma once

#include "Axiom/Data/Nbt/NbtByte.h"
#include "Axiom/Data/Nbt/NbtByteArray.h"
#include "Axiom/Data/Nbt/NbtDouble.h"
#include "Axiom/Data/Nbt/NbtFloat.h"
#include "Axiom/Data/Nbt/NbtInt.h"
#include "Axiom/Data/Nbt/NbtIntArray.h"
#include "Axiom/Data/Nbt/NbtList.h"
#include "Axiom/Data/Nbt/NbtLong.h"
#include "Axiom/Data/Nbt/NbtLongArray.h"
#include "Axiom/Data/Nbt/NbtShort.h"
#include "Axiom/Data/Nbt/NbtString.h"
#include "Axiom/Data/Nbt/NbtTag.h"
#include "Axiom/Network/NetworkBuffer.h"

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Axiom {

	/**
	 * Compound tag: a keyed map of named child tags. This is the primary
	 * container for nested NBT data (chunks, level.dat, text components).
	 */
	class NbtCompound : public NbtTag {
	public:
		NbtCompound() = default;

		NbtTagType Type() const override { return NbtTagType::Compound; }

		// ----- Query ----------------------------------------------------

		bool Contains(const std::string& name) const {
			return m_Tags.find(name) != m_Tags.end();
		}

		bool Contains(const std::string& name, const NbtTagType type) const {
			const auto iterator = m_Tags.find(name);
			return iterator != m_Tags.end() && iterator->second->Type() == type;
		}

		size_t Size() const { return m_Tags.size(); }
		bool IsEmpty() const { return m_Tags.empty(); }

		Ref<NbtTag> Get(const std::string& name) const {
			const auto iterator = m_Tags.find(name);
			return iterator != m_Tags.end() ? iterator->second : nullptr;
		}

		const std::unordered_map<std::string, Ref<NbtTag>>& Tags() const { return m_Tags; }

		// ----- Setters --------------------------------------------------

		void Put(const std::string& name, const Ref<NbtTag>& tag) {
			if (!tag) return;
			m_Tags[name] = tag;
		}

		void PutByte(const std::string& name, const int8_t value) {
			m_Tags[name] = Ref<NbtByte>::Create(value);
		}

		void PutShort(const std::string& name, const int16_t value) {
			m_Tags[name] = Ref<NbtShort>::Create(value);
		}

		void PutInt(const std::string& name, const int32_t value) {
			m_Tags[name] = Ref<NbtInt>::Create(value);
		}

		void PutLong(const std::string& name, const int64_t value) {
			m_Tags[name] = Ref<NbtLong>::Create(value);
		}

		void PutFloat(const std::string& name, const float value) {
			m_Tags[name] = Ref<NbtFloat>::Create(value);
		}

		void PutDouble(const std::string& name, const double value) {
			m_Tags[name] = Ref<NbtDouble>::Create(value);
		}

		void PutString(const std::string& name, const std::string& value) {
			m_Tags[name] = Ref<NbtString>::Create(value);
		}

		void PutBoolean(const std::string& name, const bool value) {
			m_Tags[name] = Ref<NbtByte>::Create(value ? 1 : 0);
		}

		void PutByteArray(const std::string& name, std::vector<int8_t> value) {
			m_Tags[name] = Ref<NbtByteArray>::Create(std::move(value));
		}

		void PutIntArray(const std::string& name, std::vector<int32_t> value) {
			m_Tags[name] = Ref<NbtIntArray>::Create(std::move(value));
		}

		void PutLongArray(const std::string& name, std::vector<int64_t> value) {
			m_Tags[name] = Ref<NbtLongArray>::Create(std::move(value));
		}

		void PutList(const std::string& name, const Ref<NbtList>& list) {
			if (!list) return;
			m_Tags[name] = list;
		}

		void PutCompound(const std::string& name, const Ref<NbtCompound>& compound) {
			if (!compound) return;
			m_Tags[name] = compound;
		}

		// ----- Typed getters (default return on miss/type mismatch) -----

		int8_t GetByte(const std::string& name) const {
			const auto tag = Get(name);
			if (tag && tag->Type() == NbtTagType::Byte) {
				return static_cast<NbtByte*>(tag.Raw())->Value();
			}
			return 0;
		}

		int16_t GetShort(const std::string& name) const {
			const auto tag = Get(name);
			if (tag && tag->Type() == NbtTagType::Short) {
				return static_cast<NbtShort*>(tag.Raw())->Value();
			}
			return 0;
		}

		int32_t GetInt(const std::string& name) const {
			const auto tag = Get(name);
			if (tag && tag->Type() == NbtTagType::Int) {
				return static_cast<NbtInt*>(tag.Raw())->Value();
			}
			return 0;
		}

		int64_t GetLong(const std::string& name) const {
			const auto tag = Get(name);
			if (tag && tag->Type() == NbtTagType::Long) {
				return static_cast<NbtLong*>(tag.Raw())->Value();
			}
			return 0;
		}

		float GetFloat(const std::string& name) const {
			const auto tag = Get(name);
			if (tag && tag->Type() == NbtTagType::Float) {
				return static_cast<NbtFloat*>(tag.Raw())->Value();
			}
			return 0.0F;
		}

		double GetDouble(const std::string& name) const {
			const auto tag = Get(name);
			if (tag && tag->Type() == NbtTagType::Double) {
				return static_cast<NbtDouble*>(tag.Raw())->Value();
			}
			return 0.0;
		}

		std::string GetString(const std::string& name) const {
			const auto tag = Get(name);
			if (tag && tag->Type() == NbtTagType::String) {
				return static_cast<NbtString*>(tag.Raw())->Value();
			}
			return "";
		}

		bool GetBoolean(const std::string& name) const {
			return GetByte(name) != 0;
		}

		std::vector<int8_t> GetByteArray(const std::string& name) const {
			const auto tag = Get(name);
			if (tag && tag->Type() == NbtTagType::ByteArray) {
				return static_cast<NbtByteArray*>(tag.Raw())->Value();
			}
			return {};
		}

		std::vector<int32_t> GetIntArray(const std::string& name) const {
			const auto tag = Get(name);
			if (tag && tag->Type() == NbtTagType::IntArray) {
				return static_cast<NbtIntArray*>(tag.Raw())->Value();
			}
			return {};
		}

		std::vector<int64_t> GetLongArray(const std::string& name) const {
			const auto tag = Get(name);
			if (tag && tag->Type() == NbtTagType::LongArray) {
				return static_cast<NbtLongArray*>(tag.Raw())->Value();
			}
			return {};
		}

		Ref<NbtList> GetList(const std::string& name) const {
			const auto tag = Get(name);
			if (tag && tag->Type() == NbtTagType::List) {
				return tag.As<NbtList>();
			}
			return nullptr;
		}

		Ref<NbtCompound> GetCompound(const std::string& name) const {
			const auto tag = Get(name);
			if (tag && tag->Type() == NbtTagType::Compound) {
				return tag.As<NbtCompound>();
			}
			return nullptr;
		}

		// ----- Serialization --------------------------------------------

		void Write(NetworkBuffer& buffer) const override {
			for (const auto& [name, tag] : m_Tags) {
				WriteNamedTag(buffer, name, tag);
			}
			buffer.WriteByte(0); // TAG_End
		}

		void Read(NetworkBuffer& buffer, NbtAccounter& accounter) override {
			NbtAccounter::DepthGuard depth(accounter);

			m_Tags.clear();
			while (true) {
				const auto type = static_cast<NbtTagType>(buffer.ReadByte());
				if (type == NbtTagType::End) return;

				// Read name length + body against the
				// accounter. The length is a uint16 so
				// it's naturally bounded at 65535, but
				// the budget still has to cover it to
				// prevent a thousand max-length names
				// from exhausting memory.
				const uint16_t nameLength = static_cast<uint16_t>(buffer.ReadShort()) & 0xFFFFU;
				if (!accounter.AccountBytes(nameLength)) {
					throw std::runtime_error(accounter.LastError());
				}
				std::string name(nameLength, '\0');
				for (uint16_t index = 0; index < nameLength; index++) {
					name[index] = static_cast<char>(buffer.ReadByte());
				}

				auto tag = CreateNbtTag(type);
				if (tag) {
					tag->Read(buffer, accounter);
					m_Tags[name] = tag;
				}
			}
		}

		Ref<NbtTag> Clone() const override {
			auto copy = Ref<NbtCompound>::Create();
			for (const auto& [name, tag] : m_Tags) {
				copy->m_Tags[name] = tag->Clone();
			}
			return copy;
		}

		std::string ToString() const override {
			return "{" + std::to_string(m_Tags.size()) + " entries}";
		}

	private:
		static void WriteNamedTag(NetworkBuffer& buffer, const std::string& name, const Ref<NbtTag>& tag) {
			buffer.WriteByte(static_cast<uint8_t>(tag->Type()));
			buffer.WriteUnsignedShort(static_cast<uint16_t>(name.size()));
			for (const char character : name) {
				buffer.WriteByte(static_cast<uint8_t>(character));
			}
			tag->Write(buffer);
		}

		std::unordered_map<std::string, Ref<NbtTag>> m_Tags;
	};

}
