#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Core/UUID.h"
#include "Axiom/Network/NetworkBuffer.h"

namespace Axiom {

// Type serializers for automatic packet serialization

struct VarIntSerializer {
	static int32_t Read(NetworkBuffer& buffer) { return buffer.ReadVarInt(); }
	static void Write(NetworkBuffer& buffer, const int32_t value) { buffer.WriteVarInt(value); }
};

struct VarLongSerializer {
	static int64_t Read(NetworkBuffer& buffer) { return buffer.ReadVarLong(); }
	static void Write(NetworkBuffer& buffer, const int64_t value) { buffer.WriteVarLong(value); }
};

struct IntSerializer {
	static int32_t Read(NetworkBuffer& buffer) { return buffer.ReadInt(); }
	static void Write(NetworkBuffer& buffer, const int32_t value) { buffer.WriteInt(value); }
};

struct LongSerializer {
	static int64_t Read(NetworkBuffer& buffer) { return buffer.ReadLong(); }
	static void Write(NetworkBuffer& buffer, const int64_t value) { buffer.WriteLong(value); }
};

struct FloatSerializer {
	static float Read(NetworkBuffer& buffer) { return buffer.ReadFloat(); }
	static void Write(NetworkBuffer& buffer, const float value) { buffer.WriteFloat(value); }
};

struct DoubleSerializer {
	static double Read(NetworkBuffer& buffer) { return buffer.ReadDouble(); }
	static void Write(NetworkBuffer& buffer, const double value) { buffer.WriteDouble(value); }
};

struct BoolSerializer {
	static bool Read(NetworkBuffer& buffer) { return buffer.ReadBoolean(); }
	static void Write(NetworkBuffer& buffer, const bool value) { buffer.WriteBoolean(value); }
};

struct ByteSerializer {
	static int8_t Read(NetworkBuffer& buffer) { return buffer.ReadByte(); }
	static void Write(NetworkBuffer& buffer, const int8_t value) { buffer.WriteByte(value); }
};

struct UnsignedByteSerializer {
	static uint8_t Read(NetworkBuffer& buffer) { return buffer.ReadUnsignedByte(); }
	static void Write(NetworkBuffer& buffer, uint8_t value) { buffer.WriteUnsignedByte(value); }
};

struct ShortSerializer {
	static int16_t Read(NetworkBuffer& buffer) { return buffer.ReadShort(); }
	static void Write(NetworkBuffer& buffer, const int16_t value) { buffer.WriteShort(value); }
};

struct UnsignedShortSerializer {
	static uint16_t Read(NetworkBuffer& buffer) { return buffer.ReadUnsignedShort(); }
	static void Write(NetworkBuffer& buffer, const uint16_t value) { buffer.WriteUnsignedShort(value); }
};

struct StringSerializer {
	static std::string Read(NetworkBuffer& buffer) { return buffer.ReadString(); }
	static void Write(NetworkBuffer& buffer, const std::string& value) { buffer.WriteString(value); }
};

struct UUIDSerializer {
	static UUID Read(NetworkBuffer& buffer) { return buffer.ReadUUID(); }
	static void Write(NetworkBuffer& buffer, const UUID& value) { buffer.WriteUUID(value); }
};

struct PositionSerializer {
	static BlockPos Read(NetworkBuffer& buffer) { return buffer.ReadBlockPosition(); }
	static void Write(NetworkBuffer& buffer, const BlockPos& value) { buffer.WriteBlockPosition(value); }
};

struct ChatComponentSerializer {
	static Ref<ChatComponent> Read(NetworkBuffer& buffer) { return buffer.ReadChatComponent(); }
	static void Write(NetworkBuffer& buffer, const Ref<ChatComponent>& value) { buffer.WriteChatComponent(value); }
};

struct NBTSerializer {
	static nbt::NBT Read(NetworkBuffer& buffer) { return buffer.ReadNBT(); }
	static void Write(NetworkBuffer& buffer, const nbt::NBT& value) { buffer.WriteNBT(value); }
};

} // namespace Axiom
