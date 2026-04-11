#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Core/Math.h"
#include "Axiom/Core/Error.h"

#include <stdexcept>
#include <string>
#include <vector>

namespace Axiom {

/**
 * Buffer for network packet serialization/deserialization.
 *
 * Provides methods for reading/writing Minecraft protocol data types
 * with optional Result<T> based error handling.
 */
class NetworkBuffer {
public:
	NetworkBuffer() = default;

	explicit NetworkBuffer(std::vector<uint8_t> data)
		: m_Data(std::move(data)) {}

	NetworkBuffer(const uint8_t* data, const size_t length)
		: m_Data(data, data + length) {}

	// ----- Reading (throwing versions) -----------------------------

	uint8_t ReadByte() {
		EnsureReadable(1);
		return m_Data[m_ReaderIndex++];
	}

	bool ReadBoolean() {
		return ReadByte() != 0;
	}

	int16_t ReadShort() {
		EnsureReadable(2);
		const int16_t value = static_cast<int16_t>(
			(static_cast<uint16_t>(m_Data[m_ReaderIndex]) << 8) |
			static_cast<uint16_t>(m_Data[m_ReaderIndex + 1]));
		m_ReaderIndex += 2;
		return value;
	}

	uint16_t ReadUnsignedShort() {
		EnsureReadable(2);
		const uint16_t value =
			(static_cast<uint16_t>(m_Data[m_ReaderIndex]) << 8) |
			static_cast<uint16_t>(m_Data[m_ReaderIndex + 1]);
		m_ReaderIndex += 2;
		return value;
	}

	int32_t ReadInt() {
		EnsureReadable(4);
		const int32_t value =
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
		const int32_t bits = ReadInt();
		float value;
		std::memcpy(&value, &bits, sizeof(float));
		return value;
	}

	double ReadDouble() {
		const int64_t bits = ReadLong();
		double value;
		std::memcpy(&value, &bits, sizeof(double));
		return value;
	}

	Vector2 ReadVector2() {
		const Vector2 value(ReadFloat(), ReadFloat());
		return value;
	}

	Vector3 ReadVector3() {
		const Vector3 value(ReadDouble(), ReadDouble(), ReadDouble());
		return value;
	}

	int32_t ReadVarInt() {
		int32_t value = 0;
		int position = 0;
		uint8_t currentByte;

		do {
			currentByte = ReadByte();
			value |= (currentByte & 0x7F) << position;
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

	/**
	 * Read a VarInt-length-prefixed string.
	 *
	 * The maxLength is measured in *characters*, not
	 * bytes. The on-wire byte length may be up to 3x
	 * that because each UTF-16 code unit encodes to at
	 * most 3 bytes in Modified UTF-8 (Minecraft's
	 * String wire format). The byte length is also
	 * bounded against the remaining buffer so malformed
	 * packets can never force an allocation larger
	 * than the frame itself.
	 */
	std::string ReadString(const int32_t maxLength = 0x7FFF) {
		const int32_t length = ReadVarInt();
		if (length < 0 || length > maxLength * 3) {
			throw std::runtime_error("String length out of bounds");
		}
		if (static_cast<size_t>(length) > ReadableBytes()) {
			throw std::runtime_error("String length exceeds remaining buffer");
		}
		EnsureReadable(length);
		std::string result(reinterpret_cast<const char*>(&m_Data[m_ReaderIndex]), length);
		m_ReaderIndex += length;
		return result;
	}

	std::vector<uint8_t> ReadBytes(const size_t count) {
		EnsureReadable(count);
		std::vector<uint8_t> result(m_Data.begin() + m_ReaderIndex,
			m_Data.begin() + m_ReaderIndex + count);
		m_ReaderIndex += count;
		return result;
	}

	std::vector<uint8_t> ReadRemainingBytes() {
		return ReadBytes(ReadableBytes());
	}

	std::vector<uint8_t> ReadFixedBitSet(size_t bitCount) {
		size_t byteCount = (bitCount + 7) / 8;
		return ReadBytes(byteCount);
	}

	// ----- Reading (Result<T> versions) ----------------------------

	Result<uint8_t> TryReadByte() {
		if (!CanRead(1)) {
			return MakeError<uint8_t>(ErrorCode::InvalidPacket);
		}
		return ReadByte();
	}

	Result<bool> TryReadBoolean() {
		if (!CanRead(1)) {
			return MakeError<bool>(ErrorCode::InvalidPacket);
		}
		return ReadBoolean();
	}

	Result<int32_t> TryReadVarInt() {
		int32_t value = 0;
		int position = 0;

		for (int i = 0; i < 5; i++) {
			auto byteResult = TryReadByte();
			if (!byteResult) {
				return MakeError<int32_t>(ErrorCode::InvalidPacket);
			}
			uint8_t currentByte = *byteResult;
			value |= static_cast<int32_t>(currentByte & 0x7F) << position;
			position += 7;

			if ((currentByte & 0x80) == 0) {
				return value;
			}
		}
		return MakeError<int32_t>(ErrorCode::InvalidPacket);
	}

	Result<int64_t> TryReadVarLong() {
		int64_t value = 0;
		int position = 0;

		for (int i = 0; i < 10; i++) {
			auto byteResult = TryReadByte();
			if (!byteResult) {
				return MakeError<int64_t>(ErrorCode::InvalidPacket);
			}
			uint8_t currentByte = *byteResult;
			value |= static_cast<int64_t>(currentByte & 0x7F) << position;
			position += 7;

			if ((currentByte & 0x80) == 0) {
				return value;
			}
		}
		return MakeError<int64_t>(ErrorCode::InvalidPacket);
	}

	Result<std::string> TryReadString(const int32_t maxLength = 0x7FFF) {
		auto lengthResult = TryReadVarInt();
		if (!lengthResult) {
			return MakeError<std::string>(ErrorCode::InvalidPacket);
		}
		int32_t length = *lengthResult;
		// See ReadString for the *3 rationale (Modified
		// UTF-8 worst case per UTF-16 code unit).
		if (length < 0 || length > maxLength * 3) {
			return MakeError<std::string>(ErrorCode::InvalidPacket);
		}
		if (!CanRead(length)) {
			return MakeError<std::string>(ErrorCode::InvalidPacket);
		}
		// Read exactly 'length' bytes as a string
		std::string result(reinterpret_cast<const char*>(&m_Data[m_ReaderIndex]), length);
		m_ReaderIndex += length;
		return result;
	}

	Result<std::vector<uint8_t>> TryReadBytes(const size_t count) {
		if (!CanRead(count)) {
			return MakeError<std::vector<uint8_t>>(ErrorCode::InvalidPacket);
		}
		return ReadBytes(count);
	}

	// ----- Writing -------------------------------------------------

	void WriteByte(const uint8_t value) {
		m_Data.push_back(value);
	}

	void WriteBoolean(const bool value) {
		WriteByte(value ? 1 : 0);
	}

	void WriteShort(const int16_t value) {
		m_Data.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
		m_Data.push_back(static_cast<uint8_t>(value & 0xFF));
	}

	void WriteUnsignedShort(const uint16_t value) {
		m_Data.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
		m_Data.push_back(static_cast<uint8_t>(value & 0xFF));
	}

	void WriteInt(const int32_t value) {
		m_Data.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
		m_Data.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
		m_Data.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
		m_Data.push_back(static_cast<uint8_t>(value & 0xFF));
	}

	void WriteLong(const int64_t value) {
		for (int i = 56; i >= 0; i -= 8) {
			m_Data.push_back(static_cast<uint8_t>((value >> i) & 0xFF));
		}
	}

	void WriteFloat(const float value) {
		int32_t bits;
		std::memcpy(&bits, &value, sizeof(float));
		WriteInt(bits);
	}

	void WriteDouble(const double value) {
		int64_t bits;
		std::memcpy(&bits, &value, sizeof(double));
		WriteLong(bits);
	}

	void WriteVector2(const Vector2& vector) {
		WriteFloat(vector.x);
		WriteFloat(vector.y);
	}

	void WriteVector2(const float valueX, const float valueY) {
		WriteFloat(valueX);
		WriteFloat(valueY);
	}

	void WriteVector3(const Vector3& vector) {
		WriteDouble(vector.x);
		WriteDouble(vector.y);
		WriteDouble(vector.z);
	}

	void WriteVector3(const double valueX, const double valueY, const double valueZ) {
		WriteDouble(valueX);
		WriteDouble(valueY);
		WriteDouble(valueZ);
	}

	/**
	 * Write a position as a packed 64-bit integer (MC Position type).
	 * x: 26 bits | z: 26 bits | y: 12 bits
	 */
	void WriteBlockPosition(const int32_t blockX, const int32_t blockY, const int32_t blockZ) {
		const int64_t encoded = (static_cast<int64_t>(blockX & 0x3FFFFFF) << 38)
			| (static_cast<int64_t>(blockZ & 0x3FFFFFF) << 12)
			| static_cast<int64_t>(blockY & 0xFFF);
		WriteLong(encoded);
	}

	/**
	 * Write an unnamed NBT TAG_String value: u16 length + UTF-8 bytes.
	 */
	void WriteNbtString(const std::string& value) {
		auto length = static_cast<uint16_t>(value.size());
		WriteByte(static_cast<uint8_t>((length >> 8) & 0xFF));
		WriteByte(static_cast<uint8_t>(length & 0xFF));
		m_Data.insert(m_Data.end(), value.begin(), value.end());
	}

	/**
	 * Write a text component as an NBT TAG_Compound containing
	 * a single "text" TAG_String field. This is the format used
	 * for chat messages since 1.20.3 (nameless root compound).
	 */
	void WriteTextComponent(const std::string& text) {
		WriteByte(0x0A); // TAG_Compound (nameless root)

		// Named TAG_String "text" with the message content
		WriteByte(0x08); // TAG_String
		WriteNbtString("text");
		WriteNbtString(text);

		WriteByte(0x00); // TAG_End
	}

	/**
	 * Read a packed block position.
	 */
	void ReadBlockPosition(int32_t& blockX, int32_t& blockY, int32_t& blockZ) {
		const int64_t encoded = ReadLong();
		blockX = static_cast<int32_t>(encoded >> 38);
		blockY = static_cast<int32_t>((encoded << 52) >> 52);
		blockZ = static_cast<int32_t>((encoded << 26) >> 38);
	}

	void WriteVarInt(const int32_t value) {
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

	void WriteVarLong(const int64_t value) {
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
		WriteBytes(reinterpret_cast<const uint8_t*>(value.data()), value.size());
	}

	void WriteBytes(const uint8_t* data, size_t length) {
		m_Data.insert(m_Data.end(), data, data + length);
	}

	void WriteBytes(const std::vector<uint8_t>& data) {
		m_Data.insert(m_Data.end(), data.begin(), data.end());
	}

	// ----- Utility -------------------------------------------------

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
	void ReaderIndex(const size_t index) { m_ReaderIndex = index; }
	const std::vector<uint8_t>& Data() const { return m_Data; }
	std::vector<uint8_t>& Data() { return m_Data; }
	size_t Size() const { return m_Data.size(); }
	void Clear() { m_Data.clear(); m_ReaderIndex = 0; }

	bool CanRead(size_t count) const {
		return m_ReaderIndex + count <= m_Data.size();
	}

private:
	void EnsureReadable(const size_t count) const {
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
