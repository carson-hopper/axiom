#pragma once

#include "Axiom/Data/Nbt/NbtCompound.h"
#include "Axiom/Data/Nbt/NbtIo.h"
#include "Axiom/Data/Nbt/NbtList.h"
#include "Axiom/Data/Nbt/NbtListImpl.h"
#include "Axiom/Network/NetworkBuffer.h"

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

namespace Axiom {

	/**
	 * Converts a nlohmann::json value into a proper NbtCompound tree.
	 * Used to encode extractor JSON registry data for the RegistryData packet.
	 */
	class JsonToNbt {
	public:
		/**
		 * Convert a JSON value into an NBT tag. Returns nullptr for null/invalid.
		 */
		static Ref<NbtTag> ValueToTag(const nlohmann::json& value) {
			if (value.is_object()) {
				auto compound = Ref<NbtCompound>::Create();
				for (auto& [key, val] : value.items()) {
					auto child = ValueToTag(val);
					if (child) {
						compound->Put(key, child);
					}
				}
				return compound;
			}
			if (value.is_array()) {
				return ArrayToList(value);
			}
			if (value.is_string()) {
				return Ref<NbtString>::Create(value.get<std::string>());
			}
			if (value.is_boolean()) {
				return Ref<NbtByte>::Create(value.get<bool>() ? 1 : 0);
			}
			if (value.is_number_integer()) {
				const int64_t intValue = value.get<int64_t>();
				if (intValue >= -128 && intValue <= 127) {
					return Ref<NbtByte>::Create(static_cast<int8_t>(intValue));
				}
				if (intValue >= -32768 && intValue <= 32767) {
					return Ref<NbtShort>::Create(static_cast<int16_t>(intValue));
				}
				if (intValue >= INT32_MIN && intValue <= INT32_MAX) {
					return Ref<NbtInt>::Create(static_cast<int32_t>(intValue));
				}
				return Ref<NbtLong>::Create(intValue);
			}
			if (value.is_number_float()) {
				return Ref<NbtFloat>::Create(value.get<float>());
			}
			return nullptr;
		}

		/**
		 * Convert a JSON object to a root NbtCompound.
		 */
		static Ref<NbtCompound> ObjectToCompound(const nlohmann::json& object) {
			auto compound = Ref<NbtCompound>::Create();
			for (auto& [key, val] : object.items()) {
				auto child = ValueToTag(val);
				if (child) {
					compound->Put(key, child);
				}
			}
			return compound;
		}

		/**
		 * Legacy entry point: convert a JSON object to raw NBT bytes in
		 * network format (unnamed root compound).
		 */
		static std::vector<uint8_t> ObjectToRootCompound(const nlohmann::json& object) {
			auto compound = ObjectToCompound(object);
			NetworkBuffer buffer;
			NbtIo::WriteNetwork(compound, buffer);
			return buffer.Data();
		}

	private:
		static Ref<NbtList> ArrayToList(const nlohmann::json& array) {
			if (array.empty()) {
				return Ref<NbtList>::Create(NbtTagType::End);
			}

			const NbtTagType elementType = DeduceNbtType(array[0]);
			auto list = Ref<NbtList>::Create(elementType);
			for (const auto& element : array) {
				auto child = ValueToTag(element);
				if (child && child->Type() == elementType) {
					list->Add(child);
				}
			}
			return list;
		}

		static NbtTagType DeduceNbtType(const nlohmann::json& value) {
			if (value.is_object()) return NbtTagType::Compound;
			if (value.is_string()) return NbtTagType::String;
			if (value.is_boolean()) return NbtTagType::Byte;
			if (value.is_number_integer()) {
				const int64_t intValue = value.get<int64_t>();
				if (intValue >= -128 && intValue <= 127) return NbtTagType::Byte;
				if (intValue >= -32768 && intValue <= 32767) return NbtTagType::Short;
				if (intValue >= INT32_MIN && intValue <= INT32_MAX) return NbtTagType::Int;
				return NbtTagType::Long;
			}
			if (value.is_number_float()) return NbtTagType::Float;
			if (value.is_array()) return NbtTagType::List;
			return NbtTagType::End;
		}
	};

}
