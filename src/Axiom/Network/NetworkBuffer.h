#pragma once

#include "Axiom/Core/Base.h"

#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

namespace Axiom {

	class NetworkBuffer {
	public:
		NetworkBuffer() = default;

		explicit NetworkBuffer(std::vector<uint8_t> data)
			: m_Data(std::move(data)) {}

		NetworkBuffer(const uint8_t* data, size_t length)
			: m_Data(data, data + length) {}

		// ----- Reading --------------------------------------------------

		uint8_t ReadByte() {
			EnsureReadable(1);
			return m_Data[m_ReaderIndex++];
		}

		bool ReadBoolean() {
			return ReadByte() != 0;
		}

		int16_t ReadShort() {
			EnsureReadable(2);
			int16_t value = static_cast<int16_t>(
				(static_cast<uint16_t>(m_Data[m_ReaderIndex]) << 8) |
				static_cast<uint16_t>(m_Data[m_ReaderIndex + 1]));
			m_ReaderIndex += 2;
			return value;
		}

		uint16_t ReadUnsignedShort() {
			EnsureReadable(2);
			uint16_t value =
				(static_cast<uint16_t>(m_Data[m_ReaderIndex]) << 8) |
				static_cast<uint16_t>(m_Data[m_ReaderIndex + 1]);
			m_ReaderIndex += 2;
			return value;
		}

		int32_t ReadInt() {
			EnsureReadable(4);
			int32_t value =
				(static_cast<int32_t>(m_Data[m_ReaderIndex]) << 24) |
				(static_cast<int32_t>(m_Data[m_ReaderIndex + 1]) << 16) |
				(static_cast<int32_t>(m_Data[m_ReaderIndex + 2]) << 8) |
				static_cast<int32_t>(m_Data[m_ReaderIndex + 3]);
			m_ReaderIndex += 4;
			return value;
		}

		int64_t ReadLong() {
			EnsureReadable(8);
			int64_t value = 0;
			for (int i = 0; i < 8; i++) {
				value = (value << 8) | static_cast<int64_t>(m_Data[m_ReaderIndex + i]);
			}
			m_ReaderIndex += 8;
			return value;
		}

		float ReadFloat() {
			int32_t bits = ReadInt();
			float value;
			std::memcpy(&value, &bits, sizeof(float));
			return value;
		}

		double ReadDouble() {
			int64_t bits = ReadLong();
			double value;
			std::memcpy(&value, &bits, sizeof(double));
			return value;
		}

		int32_t ReadVarInt() {
			int32_t value = 0;
			int position = 0;
			uint8_t currentByte;

			do {
				currentByte = ReadByte();
				value |= static_cast<int32_t>(currentByte & 0x7F) << position;
				position += 7;

				if (position >= 32) {
					throw std::runtime_error("VarInt is too big");
				}
			} while ((currentByte & 0x80) != 0);

			return value;
		}

		int64_t ReadVarLong() {
			int64_t value = 0;
			int position = 0;
			uint8_t currentByte;

			do {
				currentByte = ReadByte();
				value |= static_cast<int64_t>(currentByte & 0x7F) << position;
				position += 7;

				if (position >= 64) {
					throw std::runtime_error("VarLong is too big");
				}
			} while ((currentByte & 0x80) != 0);

			return value;
		}

		std::string ReadString(int32_t maxLength = 32767) {
			int32_t length = ReadVarInt();
			if (length < 0 || length > maxLength * 4) {
				throw std::runtime_error("String length out of bounds");
			}
			EnsureReadable(length);
			std::string result(reinterpret_cast<const char*>(&m_Data[m_ReaderIndex]), length);
			m_ReaderIndex += length;
			return result;
		}

		std::vector<uint8_t> ReadBytes(size_t count) {
			EnsureReadable(count);
			std::vector<uint8_t> result(m_Data.begin() + m_ReaderIndex,
				m_Data.begin() + m_ReaderIndex + count);
			m_ReaderIndex += count;
			return result;
		}

		std::vector<uint8_t> ReadRemainingBytes() {
			return ReadBytes(ReadableBytes());
		}

		// ----- Writing --------------------------------------------------

		void WriteByte(uint8_t value) {
			m_Data.push_back(value);
		}

		void WriteBoolean(bool value) {
			WriteByte(value ? 1 : 0);
		}

		void WriteShort(int16_t value) {
			m_Data.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
			m_Data.push_back(static_cast<uint8_t>(value & 0xFF));
		}

		void WriteUnsignedShort(uint16_t value) {
			m_Data.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
			m_Data.push_back(static_cast<uint8_t>(value & 0xFF));
		}

		void WriteInt(int32_t value) {
			m_Data.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
			m_Data.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
			m_Data.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
			m_Data.push_back(static_cast<uint8_t>(value & 0xFF));
		}

		void WriteLong(int64_t value) {
			for (int i = 56; i >= 0; i -= 8) {
				m_Data.push_back(static_cast<uint8_t>((value >> i) & 0xFF));
			}
		}

		void WriteFloat(float value) {
			int32_t bits;
			std::memcpy(&bits, &value, sizeof(float));
			WriteInt(bits);
		}

		void WriteDouble(double value) {
			int64_t bits;
			std::memcpy(&bits, &value, sizeof(double));
			WriteLong(bits);
		}

		void WriteVarInt(int32_t value) {
			auto unsigned_value = static_cast<uint32_t>(value);
			do {
				uint8_t temp = static_cast<uint8_t>(unsigned_value & 0x7F);
				unsigned_value >>= 7;
				if (unsigned_value != 0) {
					temp |= 0x80;
				}
				WriteByte(temp);
			} while (unsigned_value != 0);
		}

		void WriteVarLong(int64_t value) {
			auto unsigned_value = static_cast<uint64_t>(value);
			do {
				uint8_t temp = static_cast<uint8_t>(unsigned_value & 0x7F);
				unsigned_value >>= 7;
				if (unsigned_value != 0) {
					temp |= 0x80;
				}
				WriteByte(temp);
			} while (unsigned_value != 0);
		}

		void WriteString(const std::string& value) {
			WriteVarInt(static_cast<int32_t>(value.size()));
			m_Data.insert(m_Data.end(), value.begin(), value.end());
		}

		void WriteBytes(const uint8_t* data, size_t length) {
			m_Data.insert(m_Data.end(), data, data + length);
		}

		void WriteBytes(const std::vector<uint8_t>& data) {
			m_Data.insert(m_Data.end(), data.begin(), data.end());
		}

		// ----- Utility --------------------------------------------------

		static int VarIntSize(int32_t value) {
			auto unsigned_value = static_cast<uint32_t>(value);
			int size = 0;
			do {
				unsigned_value >>= 7;
				size++;
			} while (unsigned_value != 0);
			return size;
		}

		size_t ReadableBytes() const { return m_Data.size() - m_ReaderIndex; }
		size_t ReaderIndex() const { return m_ReaderIndex; }
		void ReaderIndex(size_t index) { m_ReaderIndex = index; }
		const std::vector<uint8_t>& Data() const { return m_Data; }
		std::vector<uint8_t>& Data() { return m_Data; }
		size_t Size() const { return m_Data.size(); }
		void Clear() { m_Data.clear(); m_ReaderIndex = 0; }

	private:
		void EnsureReadable(size_t count) const {
			if (m_ReaderIndex + count > m_Data.size()) {
				throw std::runtime_error("Buffer underflow: tried to read " +
					std::to_string(count) + " bytes, only " +
					std::to_string(m_Data.size() - m_ReaderIndex) + " available");
			}
		}

		std::vector<uint8_t> m_Data;
		size_t m_ReaderIndex = 0;
	};

}
