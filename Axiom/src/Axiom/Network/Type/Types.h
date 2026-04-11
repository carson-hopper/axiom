#pragma once

#include "Axiom/Core/Math.h"
#include "Axiom/Core/UUID.h"
#include "Axiom/Network/Type/NetworkType.h"

#include <cstdint>
#include <string>

namespace Axiom::Net {

// ---- VarInt --------------------------------------------------------

class VarInt : public NetworkType<int32_t> {
public:
	using NetworkType::NetworkType;

protected:
	int32_t ReadImpl(NetworkBuffer& buffer) override {
		return buffer.ReadVarInt();
	}
	void WriteImpl(NetworkBuffer& buffer) const override {
		buffer.WriteVarInt(m_Value);
	}
};

// ---- VarLong -------------------------------------------------------

class VarLong : public NetworkType<int64_t> {
public:
	using NetworkType::NetworkType;

protected:
	int64_t ReadImpl(NetworkBuffer& buffer) override {
		return buffer.ReadVarLong();
	}
	void WriteImpl(NetworkBuffer& buffer) const override {
		buffer.WriteVarLong(m_Value);
	}
};

// ---- String --------------------------------------------------------

class String : public NetworkType<std::string> {
public:
	using NetworkType::NetworkType;

protected:
	std::string ReadImpl(NetworkBuffer& buffer) override {
		return buffer.ReadString();
	}
	void WriteImpl(NetworkBuffer& buffer) const override {
		buffer.WriteString(m_Value);
	}
};

// ---- Long ----------------------------------------------------------

class Long : public NetworkType<int64_t> {
public:
	using NetworkType::NetworkType;

protected:
	int64_t ReadImpl(NetworkBuffer& buffer) override {
		return buffer.ReadLong();
	}
	void WriteImpl(NetworkBuffer& buffer) const override {
		buffer.WriteLong(m_Value);
	}
};

// ---- Int -----------------------------------------------------------

class Int : public NetworkType<int32_t> {
public:
	using NetworkType::NetworkType;

protected:
	int32_t ReadImpl(NetworkBuffer& buffer) override {
		return buffer.ReadInt();
	}
	void WriteImpl(NetworkBuffer& buffer) const override {
		buffer.WriteInt(m_Value);
	}
};

// ---- Short ---------------------------------------------------------

class Short : public NetworkType<int16_t> {
public:
	using NetworkType::NetworkType;

protected:
	int16_t ReadImpl(NetworkBuffer& buffer) override {
		return buffer.ReadShort();
	}
	void WriteImpl(NetworkBuffer& buffer) const override {
		buffer.WriteShort(m_Value);
	}
};

// ---- UnsignedShort -------------------------------------------------

class UnsignedShort : public NetworkType<uint16_t> {
public:
	using NetworkType::NetworkType;

protected:
	uint16_t ReadImpl(NetworkBuffer& buffer) override {
		return buffer.ReadUnsignedShort();
	}
	void WriteImpl(NetworkBuffer& buffer) const override {
		buffer.WriteUnsignedShort(m_Value);
	}
};

// ---- Byte ----------------------------------------------------------

class Byte : public NetworkType<int8_t> {
public:
	using NetworkType::NetworkType;

protected:
	int8_t ReadImpl(NetworkBuffer& buffer) override {
		return static_cast<int8_t>(buffer.ReadByte());
	}
	void WriteImpl(NetworkBuffer& buffer) const override {
		buffer.WriteByte(static_cast<uint8_t>(m_Value));
	}
};

// ---- UnsignedByte --------------------------------------------------

class UnsignedByte : public NetworkType<uint8_t> {
public:
	using NetworkType::NetworkType;

protected:
	uint8_t ReadImpl(NetworkBuffer& buffer) override {
		return buffer.ReadByte();
	}
	void WriteImpl(NetworkBuffer& buffer) const override {
		buffer.WriteByte(m_Value);
	}
};

// ---- Float ---------------------------------------------------------

class Float : public NetworkType<float> {
public:
	using NetworkType::NetworkType;

protected:
	float ReadImpl(NetworkBuffer& buffer) override {
		return buffer.ReadFloat();
	}
	void WriteImpl(NetworkBuffer& buffer) const override {
		buffer.WriteFloat(m_Value);
	}
};

// ---- Double --------------------------------------------------------

class Double : public NetworkType<double> {
public:
	using NetworkType::NetworkType;

protected:
	double ReadImpl(NetworkBuffer& buffer) override {
		return buffer.ReadDouble();
	}
	void WriteImpl(NetworkBuffer& buffer) const override {
		buffer.WriteDouble(m_Value);
	}
};

// ---- Bool ----------------------------------------------------------

class Bool : public NetworkType<bool> {
public:
	using NetworkType::NetworkType;

protected:
	bool ReadImpl(NetworkBuffer& buffer) override {
		return buffer.ReadBoolean();
	}
	void WriteImpl(NetworkBuffer& buffer) const override {
		buffer.WriteBoolean(m_Value);
	}
};

// ---- Position (packed block position) ------------------------------

struct BlockPos {
	int32_t X = 0;
	int32_t Y = 0;
	int32_t Z = 0;
};

class Position : public NetworkType<BlockPos> {
public:
	using NetworkType::NetworkType;

protected:
	BlockPos ReadImpl(NetworkBuffer& buffer) override {
		BlockPos result;
		buffer.ReadBlockPosition(result.X, result.Y, result.Z);
		return result;
	}
	void WriteImpl(NetworkBuffer& buffer) const override {
		buffer.WriteBlockPosition(m_Value.X, m_Value.Y, m_Value.Z);
	}
};

// ---- Vec3 (3 doubles) ----------------------------------------------

class Vec3 : public NetworkType<Vector3> {
public:
	using NetworkType::NetworkType;

protected:
	Vector3 ReadImpl(NetworkBuffer& buffer) override {
		double x = buffer.ReadDouble();
		double y = buffer.ReadDouble();
		double z = buffer.ReadDouble();
		return {x, y, z};
	}
	void WriteImpl(NetworkBuffer& buffer) const override {
		buffer.WriteDouble(m_Value.x);
		buffer.WriteDouble(m_Value.y);
		buffer.WriteDouble(m_Value.z);
	}
};

// ---- Vec2 (2 floats: yaw, pitch) -----------------------------------

class Vec2 : public NetworkType<Vector2> {
public:
	using NetworkType::NetworkType;

protected:
	Vector2 ReadImpl(NetworkBuffer& buffer) override {
		float x = buffer.ReadFloat();
		float y = buffer.ReadFloat();
		return {x, y};
	}
	void WriteImpl(NetworkBuffer& buffer) const override {
		buffer.WriteFloat(m_Value.x);
		buffer.WriteFloat(m_Value.y);
	}
};

// ---- UUID (128-bit, two longs) -------------------------------------

class UuidType : public NetworkType<UUID> {
public:
	using NetworkType::NetworkType;

protected:
	UUID ReadImpl(NetworkBuffer& buffer) override {
		auto most = static_cast<uint64_t>(buffer.ReadLong());
		auto least = static_cast<uint64_t>(buffer.ReadLong());
		return {most, least};
	}
	void WriteImpl(NetworkBuffer& buffer) const override {
		buffer.WriteLong(static_cast<int64_t>(m_Value.MostSignificantBits()));
		buffer.WriteLong(static_cast<int64_t>(m_Value.LeastSignificantBits()));
	}
};

// ---- ByteArray (VarInt length-prefixed) ----------------------------

class ByteArray : public NetworkType<std::vector<uint8_t>> {
public:
	using NetworkType::NetworkType;

	/**
	 * Hard upper bound on a single ByteArray field.
	 * Matches the protocol's general string / array
	 * max (32767 bytes). Per-field tighter caps should
	 * be added for pre-auth fields (RSA ciphertext is
	 * 256 bytes for a 2048-bit key) once a bounded
	 * ByteArray wrapper exists.
	 */
	static constexpr int32_t MaxLength = 0x7FFF;

protected:
	std::vector<uint8_t> ReadImpl(NetworkBuffer& buffer) override {
		const int32_t length = buffer.ReadVarInt();
		if (length < 0 || length > MaxLength) {
			throw std::runtime_error("ByteArray length out of bounds");
		}
		return buffer.ReadBytes(static_cast<size_t>(length));
	}
	void WriteImpl(NetworkBuffer& buffer) const override {
		buffer.WriteVarInt(static_cast<int32_t>(m_Value.size()));
		buffer.WriteBytes(m_Value);
	}
};

}
