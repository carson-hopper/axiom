#pragma once

#include "Axiom/Core/Base.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace Axiom {

	/**
	 * Writes Named Binary Tag (NBT) data to a byte buffer.
	 *
	 * Supports the full NBT spec used by the Minecraft protocol:
	 * TAG_End(0), TAG_Byte(1), TAG_Short(2), TAG_Int(3), TAG_Long(4),
	 * TAG_Float(5), TAG_Double(6), TAG_Byte_Array(7), TAG_String(8),
	 * TAG_List(9), TAG_Compound(10), TAG_Int_Array(11), TAG_Long_Array(12).
	 */
	class NbtWriter {
	public:
		// ----- Compound ------------------------------------------------

		void BeginCompound(const std::string& name = "") {
			WriteTagHeader(10, name);
		}

		void BeginRootCompound() {
			// Network NBT: root compound has no name, just type byte
			WriteByte(10);
		}

		void EndCompound() {
			WriteByte(0);  // TAG_End
		}

		// ----- Primitives ----------------------------------------------

		void WriteByte(const std::string& name, int8_t value) {
			WriteTagHeader(1, name);
			WriteByte(static_cast<uint8_t>(value));
		}

		void WriteShort(const std::string& name, int16_t value) {
			WriteTagHeader(2, name);
			WriteShortRaw(value);
		}

		void WriteInt(const std::string& name, int32_t value) {
			WriteTagHeader(3, name);
			WriteIntRaw(value);
		}

		void WriteLong(const std::string& name, int64_t value) {
			WriteTagHeader(4, name);
			WriteLongRaw(value);
		}

		void WriteFloat(const std::string& name, float value) {
			WriteTagHeader(5, name);
			int32_t bits;
			std::memcpy(&bits, &value, sizeof(float));
			WriteIntRaw(bits);
		}

		void WriteDouble(const std::string& name, double value) {
			WriteTagHeader(6, name);
			int64_t bits;
			std::memcpy(&bits, &value, sizeof(double));
			WriteLongRaw(bits);
		}

		void WriteString(const std::string& name, const std::string& value) {
			WriteTagHeader(8, name);
			WriteStringRaw(value);
		}

		// ----- Lists ---------------------------------------------------

		void BeginList(const std::string& name, int8_t elementType, int32_t count) {
			WriteTagHeader(9, name);
			WriteByte(static_cast<uint8_t>(elementType));
			WriteIntRaw(count);
		}

		// For list elements (no tag header, just the value)
		void WriteListCompound() {
			// Compound list elements don't need a header, just write contents + TAG_End
		}

		// ----- Arrays --------------------------------------------------

		void WriteByteArray(const std::string& name, const std::vector<int8_t>& value) {
			WriteTagHeader(7, name);
			WriteIntRaw(static_cast<int32_t>(value.size()));
			for (auto byte : value) {
				WriteByte(static_cast<uint8_t>(byte));
			}
		}

		void WriteIntArray(const std::string& name, const std::vector<int32_t>& value) {
			WriteTagHeader(11, name);
			WriteIntRaw(static_cast<int32_t>(value.size()));
			for (auto integer : value) {
				WriteIntRaw(integer);
			}
		}

		void WriteLongArray(const std::string& name, const std::vector<int64_t>& value) {
			WriteTagHeader(12, name);
			WriteIntRaw(static_cast<int32_t>(value.size()));
			for (auto longValue : value) {
				WriteLongRaw(longValue);
			}
		}

		// ----- Boolean (stored as byte) --------------------------------

		void WriteBoolean(const std::string& name, bool value) {
			WriteByte(name, value ? 1 : 0);
		}

		// ----- Raw writes for list elements ----------------------------

		void WriteByteRaw(int8_t value) { WriteByte(static_cast<uint8_t>(value)); }
		void WriteShortValue(int16_t value) { WriteShortRaw(value); }
		void WriteIntValue(int32_t value) { WriteIntRaw(value); }
		void WriteLongValue(int64_t value) { WriteLongRaw(value); }
		void WriteFloatValue(float value) {
			int32_t bits;
			std::memcpy(&bits, &value, sizeof(float));
			WriteIntRaw(bits);
		}
		void WriteDoubleValue(double value) {
			int64_t bits;
			std::memcpy(&bits, &value, sizeof(double));
			WriteLongRaw(bits);
		}
		void WriteStringValue(const std::string& value) { WriteStringRaw(value); }

		// ----- Output --------------------------------------------------

		const std::vector<uint8_t>& Data() const { return m_Data; }
		std::vector<uint8_t>& Data() { return m_Data; }
		size_t Size() const { return m_Data.size(); }

	private:
		void WriteByte(uint8_t value) {
			m_Data.push_back(value);
		}

		void WriteShortRaw(int16_t value) {
			m_Data.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
			m_Data.push_back(static_cast<uint8_t>(value & 0xFF));
		}

		void WriteIntRaw(int32_t value) {
			m_Data.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
			m_Data.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
			m_Data.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
			m_Data.push_back(static_cast<uint8_t>(value & 0xFF));
		}

		void WriteLongRaw(int64_t value) {
			for (int i = 56; i >= 0; i -= 8) {
				m_Data.push_back(static_cast<uint8_t>((value >> i) & 0xFF));
			}
		}

		void WriteStringRaw(const std::string& value) {
			WriteShortRaw(static_cast<int16_t>(value.size()));
			m_Data.insert(m_Data.end(), value.begin(), value.end());
		}

		void WriteTagHeader(uint8_t type, const std::string& name) {
			WriteByte(type);
			WriteStringRaw(name);
		}

		std::vector<uint8_t> m_Data;
	};

}
