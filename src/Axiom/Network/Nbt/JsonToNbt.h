#pragma once

#include "Axiom/Network/Nbt/NbtWriter.h"

#include <nlohmann/json.hpp>

#include <string>

namespace Axiom {

	/**
	 * Converts a nlohmann::json value into NBT bytes using NbtWriter.
	 * Used to encode extractor JSON registry data for the RegistryData packet.
	 */
	class JsonToNbt {
	public:
		/**
		 * Write a JSON value as a named NBT tag into the writer.
		 */
		static void WriteNamedValue(NbtWriter& writer, const std::string& name,
			const nlohmann::json& value) {

			if (value.is_object()) {
				writer.BeginCompound(name);
				for (auto& [key, val] : value.items()) {
					WriteNamedValue(writer, key, val);
				}
				writer.EndCompound();
			} else if (value.is_array()) {
				WriteList(writer, name, value);
			} else if (value.is_string()) {
				writer.WriteString(name, value.get<std::string>());
			} else if (value.is_boolean()) {
				writer.WriteBoolean(name, value.get<bool>());
			} else if (value.is_number_integer()) {
				int64_t intValue = value.get<int64_t>();
				if (intValue >= -128 && intValue <= 127) {
					writer.WriteByte(name, static_cast<int8_t>(intValue));
				} else if (intValue >= -32768 && intValue <= 32767) {
					writer.WriteShort(name, static_cast<int16_t>(intValue));
				} else if (intValue >= INT32_MIN && intValue <= INT32_MAX) {
					writer.WriteInt(name, static_cast<int32_t>(intValue));
				} else {
					writer.WriteLong(name, intValue);
				}
			} else if (value.is_number_float()) {
				double floatValue = value.get<double>();
				// Use float if it fits without precision loss
				float asFloat = static_cast<float>(floatValue);
				if (static_cast<double>(asFloat) == floatValue) {
					writer.WriteFloat(name, asFloat);
				} else {
					writer.WriteDouble(name, floatValue);
				}
			}
		}

		/**
		 * Write a JSON object as a root compound (network NBT format).
		 */
		static std::vector<uint8_t> ObjectToRootCompound(const nlohmann::json& object) {
			NbtWriter writer;
			writer.BeginRootCompound();
			for (auto& [key, val] : object.items()) {
				WriteNamedValue(writer, key, val);
			}
			writer.EndCompound();
			return writer.Data();
		}

	private:
		static void WriteList(NbtWriter& writer, const std::string& name,
			const nlohmann::json& array) {

			if (array.empty()) {
				writer.BeginList(name, 0, 0);  // TAG_End type, empty
				return;
			}

			// Determine element type from first element
			const auto& first = array[0];
			int8_t elementType = DeduceNbtType(first);

			writer.BeginList(name, elementType, static_cast<int32_t>(array.size()));
			for (const auto& element : array) {
				WriteListElement(writer, element, elementType);
			}
		}

		static int8_t DeduceNbtType(const nlohmann::json& value) {
			if (value.is_object()) return 10;   // TAG_Compound
			if (value.is_string()) return 8;    // TAG_String
			if (value.is_boolean()) return 1;   // TAG_Byte
			if (value.is_number_integer()) {
				int64_t intValue = value.get<int64_t>();
				if (intValue >= -128 && intValue <= 127) return 1;
				if (intValue >= -32768 && intValue <= 32767) return 2;
				if (intValue >= INT32_MIN && intValue <= INT32_MAX) return 3;
				return 4;
			}
			if (value.is_number_float()) return 5; // TAG_Float
			return 0;
		}

		static void WriteListElement(NbtWriter& writer, const nlohmann::json& value,
			int8_t elementType) {

			switch (elementType) {
				case 1:  // TAG_Byte
					if (value.is_boolean()) {
						writer.WriteByteRaw(value.get<bool>() ? 1 : 0);
					} else {
						writer.WriteByteRaw(value.get<int8_t>());
					}
					break;
				case 2:  // TAG_Short
					writer.WriteShortValue(value.get<int16_t>());
					break;
				case 3:  // TAG_Int
					writer.WriteIntValue(value.get<int32_t>());
					break;
				case 4:  // TAG_Long
					writer.WriteLongValue(value.get<int64_t>());
					break;
				case 5:  // TAG_Float
					writer.WriteFloatValue(value.get<float>());
					break;
				case 6:  // TAG_Double
					writer.WriteDoubleValue(value.get<double>());
					break;
				case 8:  // TAG_String
					writer.WriteStringValue(value.get<std::string>());
					break;
				case 10: // TAG_Compound
					for (auto& [key, val] : value.items()) {
						WriteNamedValue(writer, key, val);
					}
					writer.EndCompound();
					break;
				default:
					break;
			}
		}
	};

}
