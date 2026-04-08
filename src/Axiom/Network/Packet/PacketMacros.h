#pragma once

/**
 * @file PacketMacros.h
 *
 * Macros for reducing boilerplate in packet implementations.
 */

#include "Axiom/Network/Packet/ServerboundPacket.h"
#include "Axiom/Network/Protocol.h"

// ----- Packet Declaration Macros --------------------------------

/**
 * Begins a serverbound packet class declaration.
 *
 * @param name The class name for the packet
 * @param state Connection state (Handshake, Status, Login, Configuration, Play)
 * @param id The packet ID constant from Protocol.h
 */
#define PACKET_DECL_BEGIN(name, state, id) \
	template<int32_t Version = PROTOCOL_VERSION> \
	class name : public ServerboundPacket { \
	public: \
		static constexpr int32_t PacketId = id; \
		static constexpr ConnectionState PacketState = ConnectionState::state; \
	\
		void Decode(NetworkBuffer& buffer) override; \
		void Handle(Ref<Connection> connection, PacketContext& context) override; \
	\
	private:

/**
 * Ends a packet class declaration.
 */
#define PACKET_DECL_END() \
	};

// ----- Field Declaration Macros ---------------------------------

#define PACKET_FIELD(type, name, v) \
	public: \
		type Get##name() const { return m_##name; } \
		void Set##name(type value) { m_##name = value; } \
	private: \
		type m_##name = v;

#define PACKET_FIELD_INT(name) PACKET_FIELD(int, name, 0)
#define PACKET_FIELD_INT8(name) PACKET_FIELD(int8_t, name, 0)
#define PACKET_FIELD_INT16(name) PACKET_FIELD(int16_t, name, 0)
#define PACKET_FIELD_INT32(name) PACKET_FIELD(int32_t, name, 0)
#define PACKET_FIELD_INT64(name) PACKET_FIELD(int64_t, name, 0)
#define PACKET_FIELD_UINT8(name) PACKET_FIELD(uint8_t, name, 0)
#define PACKET_FIELD_UINT16(name) PACKET_FIELD(uint16_t, name, 0)
#define PACKET_FIELD_UINT32(name) PACKET_FIELD(uint32_t, name, 0)
#define PACKET_FIELD_UINT64(name) PACKET_FIELD(uint64_t, name, 0)
#define PACKET_FIELD_FLOAT(name) PACKET_FIELD(float, name, 0)
#define PACKET_FIELD_DOUBLE(name) PACKET_FIELD(double, name, 0)
#define PACKET_FIELD_BOOL(name) PACKET_FIELD(bool, name, false)

#define PACKET_FIELD_STRING(name) \
	public: \
		const std::string& Get##name() const { return m_##name; } \
		void Set##name(const std::string& value) { m_##name = value; } \
	private: \
		std::string m_##name;

#define PACKET_FIELD_COORDS(basename) \
	PACKET_FIELD_INT32(basename##X) \
	PACKET_FIELD_INT32(basename##Y) \
	PACKET_FIELD_INT32(basename##Z)

// ----- Implementation Macros ------------------------------------

#define PACKET_DECODE_BEGIN(name) template<int32_t Version> void name<Version>::Decode(NetworkBuffer& buffer) {

#define PACKET_DECODE_END() }

#define PACKET_HANDLE_BEGIN(name) template<int32_t Version> void name<Version>::Handle(Ref<Connection> connection, PacketContext& context) {

#define PACKET_HANDLE_END() }

#define PACKET_INSTANTIATE(name, version) template class name<version>;

// ----- Read Operations ------------------------------------------

#define READ_VARINT(var) var = buffer.ReadVarInt()
#define READ_INT64(var) var = buffer.ReadLong()
#define READ_BYTE(var) var = buffer.ReadByte()
#define READ_SHORT(var) var = buffer.ReadShort()
#define READ_USHORT(var) var = buffer.ReadUnsignedShort()
#define READ_FLOAT(var) var = buffer.ReadFloat()
#define READ_DOUBLE(var) var = buffer.ReadDouble()
#define READ_BOOL(var) var = buffer.ReadBoolean()
#define READ_STRING(var) var = buffer.ReadString()
#define READ_STRING_MAX(var, maxLen) var = buffer.ReadString(maxLen)
#define READ_BLOCK_POS(x, y, z) buffer.ReadBlockPosition(x, y, z)

// ----- Simple Packet (no fields) --------------------------------

#define DEFINE_SIMPLE_PACKET(name, state, id) \
	template<int32_t Version = PROTOCOL_VERSION> \
	class name : public ServerboundPacket { \
	public: \
		static constexpr int32_t PacketId = id; \
		static constexpr ConnectionState PacketState = ConnectionState::state; \
	\
		void Decode(NetworkBuffer& /*buffer*/) override {} \
		void Handle(Ref<Connection> connection, PacketContext& context) override; \
	};
// ----- Clientbound Packet Declaration Macros --------------------

/**
 * Begins a clientbound packet class declaration.
 *
 * Clientbound packets are sent from server to client.
 * They have Encode() instead of Decode()/Handle().
 *
 * @param name The class name for the packet
 * @param state Connection state (Handshake, Status, Login, Configuration, Play)
 * @param id The packet ID constant from Protocol.h
 */
#define CLIENTBOUND_PACKET_DECL_BEGIN(name, state, id) \
	template<int32_t Version = PROTOCOL_VERSION> \
	class name { \
	public: \
		static constexpr int32_t PacketId = id; \
		static constexpr ConnectionState PacketState = ConnectionState::state; \
		void Encode(NetworkBuffer& buffer) const; \
	private:

/**
 * Ends a clientbound packet class declaration.
 */
#define CLIENTBOUND_PACKET_DECL_END() };

// ----- Clientbound Implementation Macros ------------------------

#define CLIENTBOUND_PACKET_ENCODE_BEGIN(name) template<int32_t Version> void name<Version>::Encode(NetworkBuffer& buffer) const {

#define CLIENTBOUND_PACKET_ENCODE_END() }

#define CLIENTBOUND_PACKET_INSTANTIATE(name, version) template class name<version>;

// ----- Clientbound Simple Packet (no fields) ----------------------

#define CLIENTBOUND_DEFINE_SIMPLE_PACKET(name, state, id) \
	template<int32_t Version = PROTOCOL_VERSION> \
	class name { \
	public: \
		static constexpr int32_t PacketId = id; \
		static constexpr ConnectionState PacketState = ConnectionState::state; \
		void Encode(NetworkBuffer& /*buffer*/) const {} \
	};

// ----- Write Operations (for clientbound packets) ---------------

#define WRITE_VARINT(val) buffer.WriteVarInt(val)
#define WRITE_INT64(val) buffer.WriteLong(val)
#define WRITE_BYTE(val) buffer.WriteByte(val)
#define WRITE_SHORT(val) buffer.WriteShort(val)
#define WRITE_USHORT(val) buffer.WriteUnsignedShort(val)
#define WRITE_FLOAT(val) buffer.WriteFloat(val)
#define WRITE_DOUBLE(val) buffer.WriteDouble(val)
#define WRITE_BOOL(val) buffer.WriteBoolean(val)
#define WRITE_STRING(val) buffer.WriteString(val)
#define WRITE_BYTES(val) buffer.WriteBytes(val)
#define WRITE_BLOCK_POS(x, y, z) buffer.WriteBlockPosition(x, y, z)
