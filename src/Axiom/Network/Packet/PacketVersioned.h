#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Network/Protocol.h"
#include "Axiom/Network/NetworkBuffer.h"
#include "Axiom/Network/Packet/ServerboundPacket.h"
#include "Axiom/Network/Packet/ClientboundPacket.h"

#include <array>
#include <cstdint>
#include <optional>
#include <vector>

// Helper macros for token concatenation (double-indirection for nested expansion)
#define PP_CAT_IMPL(a, b) a ## b
#define PP_CAT(a, b) PP_CAT_IMPL(a, b)
#define PP_STRINGIFY(x) #x

// Member variable prefix - creates mFieldName from FieldName
#define FIELD_MEMBER_NAME(name) PP_CAT(m_, name)

// Version-aware packet declaration
// Usage: PACKET_VERSIONED_SB(Name, ProtocolVersion, Direction, State, PacketId)
// Direction: SERVERBOUND (maps to ServerboundPacket) or CLIENTBOUND (maps to ClientboundPacket)
#define PACKET_VERSIONED_COMMON(name, version, dir, state, id) \
	template<int32_t Version> class name; \
	template<> class name<version> : public PP_CAT(dir, Packet) { \
    public: \
        static constexpr int32_t PacketId = id; \
        static constexpr int32_t ProtocolVersion = version; \
        static constexpr ConnectionState PacketState = ConnectionState::state; \
        using PacketType = name<version>; \
        \
        name() = default; \
        name(name&&) = default; \
        name& operator=(name&&) = default; \
        \
        int32_t GetPacketId() const override { return PacketId; } \

#define PACKET_VERSIONED_SB(name, version, state, id) \
		PACKET_VERSIONED_COMMON(name, version, Serverbound, state, id) \
	public: \
        void Decode(NetworkBuffer& buffer) override; \
        void Handle(Ref<Connection> connection, PacketContext& context) override; \
        \
    private:

#define PACKET_VERSIONED_CB(name, version, state, id) \
		PACKET_VERSIONED_COMMON(name, version, Clientbound, state, id) \
	public: \
        void Encode(NetworkBuffer& buffer) const override; \
        \
    private:

#define PACKET_VERSIONED_END(name, version) \
    }; \
    template class name<version>;

// Field declaration - generates member variable and accessors
#define FIELD(type, name, v) \
    public: \
        type Get##name() const { return FIELD_MEMBER_NAME(name); } \
        void Set##name(type value) { FIELD_MEMBER_NAME(name) = std::move(value); } \
    private: \
        type FIELD_MEMBER_NAME(name) = v;

#define FIELD_STRING(name) \
    public: \
        const std::string& Get##name() const { return FIELD_MEMBER_NAME(name); } \
        void Set##name(std::string value) { FIELD_MEMBER_NAME(name) = std::move(value); } \
    private: \
        std::string FIELD_MEMBER_NAME(name);

// Field with custom getter name (for booleans: IsField vs GetField)
#define FIELD_BOOL(name) \
    public: \
        bool Is##name() const { return FIELD_MEMBER_NAME(name); } \
        void Set##name(bool value) { FIELD_MEMBER_NAME(name) = value; } \
    private: \
        bool FIELD_MEMBER_NAME(name) = false;

// Field added in a specific version (for protocol evolution)
// Usage: FIELD_NEW(780, Bool, filtered) means "filtered" field added in version 780
#define FIELD_NEW(added_version, type, name, v) \
    static_assert(ProtocolVersion >= added_version, "Field added in version " #added_version); \
    FIELD(type, name, v)

// Optional field wrapper
#define FIELD_OPTIONAL(type, name) FIELD(std::optional<type>, name, {})

// Array field wrapper
#define FIELD_ARRAY(type, name) \
	public: \
        std::vector<type> Get##name() const { return FIELD_MEMBER_NAME(name); } \
        void Set##name(std::vector<type> value) { FIELD_MEMBER_NAME(name) = std::move(value); } \
    private: \
        std::vector<type> FIELD_MEMBER_NAME(name);

// Fixed-size array field (expanded inline to avoid comma-in-template macro issue)
#define FIELD_BYTE_ARRAY(size, name) \
    public: \
        std::array<uint8_t, size> Get##name() const { return FIELD_MEMBER_NAME(name); } \
        void Set##name(std::array<uint8_t, size> value) { FIELD_MEMBER_NAME(name) = value; } \
    private: \
        std::array<uint8_t, size> FIELD_MEMBER_NAME(name) = std::array<uint8_t, size>{};

// ---- Implementation macros for .cpp files ---------------------------
// Used in packet .cpp files to define Decode/Handle/Encode method bodies.
// Currently targets protocol version 775. When multiple versions are
// supported, pass the version explicitly via the _V variants.

#define PACKET_DECODE_BEGIN(name) \
    void name<775>::Decode(NetworkBuffer& buffer) {

#define PACKET_DECODE_END() }

#define PACKET_HANDLE_BEGIN(name) \
    void name<775>::Handle( \
        Ref<Connection> connection, PacketContext& context) { \
        (void)connection; (void)context;

#define PACKET_HANDLE_END() }

#define CLIENTBOUND_PACKET_ENCODE_BEGIN(name) \
    void name<775>::Encode(NetworkBuffer& buffer) const {

#define CLIENTBOUND_PACKET_ENCODE_END() }

#define PACKET_INSTANTIATE(name, version) /* handled by PACKET_VERSIONED_END */

// ---- Buffer read macros ---------------------------------------------

#define READ_VARINT(field) field = buffer.ReadVarInt()
#define READ_BYTE(field) field = buffer.ReadByte()
#define READ_BOOL(field) field = buffer.ReadBoolean()
#define READ_STRING(field) field = buffer.ReadString()
#define READ_STRING_MAX(field, max) field = buffer.ReadString(max)
#define READ_LONG(field) field = buffer.ReadLong()
#define READ_INT64(field) field = buffer.ReadLong()
#define READ_SHORT(field) field = buffer.ReadShort()
#define READ_UNSIGNED_SHORT(field) field = buffer.ReadUnsignedShort()
#define READ_USHORT(field) field = buffer.ReadUnsignedShort()
#define READ_INT(field) field = buffer.ReadInt()
#define READ_FLOAT(field) field = buffer.ReadFloat()
#define READ_DOUBLE(field) field = buffer.ReadDouble()
#define READ_UUID(field) field = buffer.ReadUUID()
#define READ_POSITION(field) field = buffer.ReadBlockPosition()
#define READ_BLOCK_POS(fieldX, fieldY, fieldZ) buffer.ReadBlockPosition(fieldX, fieldY, fieldZ)

// ---- Buffer write macros --------------------------------------------

#define WRITE_VARINT(value) buffer.WriteVarInt(value)
#define WRITE_BYTE(value) buffer.WriteByte(value)
#define WRITE_BOOL(value) buffer.WriteBoolean(value)
#define WRITE_STRING(value) buffer.WriteString(value)
#define WRITE_LONG(value) buffer.WriteLong(value)
#define WRITE_SHORT(value) buffer.WriteShort(value)
#define WRITE_INT(value) buffer.WriteInt(value)
#define WRITE_FLOAT(value) buffer.WriteFloat(value)
#define WRITE_DOUBLE(value) buffer.WriteDouble(value)
#define WRITE_UUID(value) buffer.WriteUUID(value)
#define WRITE_POSITION(value) buffer.WriteBlockPosition(value)

// ---- Usage example --------------------------------------------------
/*
namespace Axiom {

// Protocol 775 (Minecraft 1.21)
PACKET_VERSIONED_SB(PlayerChatPacket, 775, Clientbound, Play, 0x41)
    FIELD(UUID, SenderUuid)
    FIELD_OPTIONAL(std::vector<uint8_t>, Signature)
    FIELD(Ref<ChatComponent>, Message)
    FIELD(int64_t, Timestamp)
    FIELD(int64_t, Salt)
    FIELD_ARRAY(int32_t, PreviousMessages)
    FIELD_OPTIONAL(Ref<ChatComponent>, UnsignedContent)
    FIELD(int32_t, FilterType)
    FIELD(int32_t, ChatType)
    FIELD(std::string, SenderName)
    FIELD_OPTIONAL(std::string, TargetName)
PACKET_VERSIONED_END()

// Protocol 780 (Minecraft 1.21.1) adds filtered field
PACKET_VERSIONED_SB(PlayerChatPacket, 780, Clientbound, Play, 0x44)
    FIELD(UUID, SenderUuid)
    FIELD_OPTIONAL(std::vector<uint8_t>, Signature)
    FIELD(Ref<ChatComponent>, Message)
    FIELD(int64_t, Timestamp)
    FIELD(int64_t, Salt)
    FIELD_ARRAY(int32_t, PreviousMessages)
    FIELD_OPTIONAL(Ref<ChatComponent>, UnsignedContent)
    FIELD(int32_t, FilterType)
    FIELD(int32_t, ChatType)
    FIELD(std::string, SenderName)
    FIELD_OPTIONAL(std::string, TargetName)
    FIELD_NEW(780, bool, Filtered)  // Added in version 780
PACKET_VERSIONED_END()

} // namespace Axiom
*/
