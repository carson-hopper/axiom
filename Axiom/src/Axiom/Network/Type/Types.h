#pragma once

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

}
