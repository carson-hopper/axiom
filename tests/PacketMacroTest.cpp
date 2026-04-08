#include "TestFramework.h"

#include "Axiom/Network/Packet/PacketMacros.h"
#include "Axiom/Network/NetworkBuffer.h"

using namespace Axiom;

// Test packet using macros
PACKET_DECL_BEGIN(TestPacket, Play, 0x01)
	PACKET_FIELD_INT32(TestInt)
	PACKET_FIELD_STRING(TestString)
	PACKET_FIELD_BOOL(TestBool)
PACKET_DECL_END()

PACKET_DECODE_BEGIN(TestPacket)
	READ_VARINT(m_TestInt);
	READ_STRING(m_TestString);
	READ_BOOL(m_TestBool);
PACKET_DECODE_END()

PACKET_INSTANTIATE(TestPacket, 775)

TEST(PacketMacroTest, FieldDeclaration) {
	TestPacket<775> packet;
	
	// Test default values
	ASSERT_EQ(packet.GetTestInt(), 0);
	ASSERT_EQ(packet.GetTestString(), "");
	ASSERT_EQ(packet.GetTestBool(), false);
	
	// Test setters
	packet.SetTestInt(42);
	packet.SetTestString("Hello");
	packet.SetTestBool(true);
	
	ASSERT_EQ(packet.GetTestInt(), 42);
	ASSERT_EQ(packet.GetTestString(), "Hello");
	ASSERT_EQ(packet.GetTestBool(), true);
}

TEST(PacketMacroTest, PacketConstants) {
	ASSERT_EQ(TestPacket<775>::PacketId, 0x01);
	ASSERT_TRUE(TestPacket<775>::PacketState == ConnectionState::Play);
}

TEST(PacketMacroTest, DecodeOperation) {
	// Create a buffer with test data
	NetworkBuffer buffer;
	buffer.WriteVarInt(100);
	buffer.WriteString("Test message");
	buffer.WriteBoolean(true);
	
	TestPacket<775> packet;
	packet.Decode(buffer);
	
	ASSERT_EQ(packet.GetTestInt(), 100);
	ASSERT_EQ(packet.GetTestString(), "Test message");
	ASSERT_EQ(packet.GetTestBool(), true);
}

// Test coordinate fields
PACKET_DECL_BEGIN(CoordPacket, Play, 0x02)
	PACKET_FIELD_COORDS(Position)
PACKET_DECL_END()

PACKET_DECODE_BEGIN(CoordPacket)
	READ_BLOCK_POS(m_PositionX, m_PositionY, m_PositionZ);
PACKET_DECODE_END()

PACKET_INSTANTIATE(CoordPacket, 775)

TEST(PacketMacroTest, CoordFields) {
	CoordPacket<775> packet;
	
	// Test individual coordinate setters
	packet.SetPositionX(10);
	packet.SetPositionY(64);
	packet.SetPositionZ(-5);
	
	ASSERT_EQ(packet.GetPositionX(), 10);
	ASSERT_EQ(packet.GetPositionY(), 64);
	ASSERT_EQ(packet.GetPositionZ(), -5);
}

TEST(PacketMacroTest, CoordDecode) {
	NetworkBuffer buffer;
	buffer.WriteBlockPosition(100, 70, 200);
	
	CoordPacket<775> packet;
	packet.Decode(buffer);
	
	ASSERT_EQ(packet.GetPositionX(), 100);
	ASSERT_EQ(packet.GetPositionY(), 70);
	ASSERT_EQ(packet.GetPositionZ(), 200);
}

// Test all field types
PACKET_DECL_BEGIN(AllTypesPacket, Play, 0x03)
	PACKET_FIELD_INT8(Int8Val)
	PACKET_FIELD_INT16(Int16Val)
	PACKET_FIELD_INT32(Int32Val)
	PACKET_FIELD_INT64(Int64Val)
	PACKET_FIELD_UINT8(UInt8Val)
	PACKET_FIELD_UINT16(UInt16Val)
	PACKET_FIELD_FLOAT(FloatVal)
	PACKET_FIELD_DOUBLE(DoubleVal)
PACKET_DECL_END()

PACKET_DECODE_BEGIN(AllTypesPacket)
	m_Int8Val = buffer.ReadByte();
	m_Int16Val = buffer.ReadShort();
	m_Int32Val = buffer.ReadVarInt();
	m_Int64Val = buffer.ReadLong();
	m_UInt8Val = buffer.ReadByte();
	m_UInt16Val = buffer.ReadUnsignedShort();
	m_FloatVal = buffer.ReadFloat();
	m_DoubleVal = buffer.ReadDouble();
PACKET_DECODE_END()

PACKET_INSTANTIATE(AllTypesPacket, 775)

TEST(PacketMacroTest, AllFieldTypes) {
	NetworkBuffer buffer;
	buffer.WriteByte(-5);
	buffer.WriteShort(-1000);
	buffer.WriteVarInt(50000);
	buffer.WriteLong(9999999999LL);
	buffer.WriteByte(255);
	buffer.WriteUnsignedShort(60000);
	buffer.WriteFloat(3.14f);
	buffer.WriteDouble(2.718281828);
	
	AllTypesPacket<775> packet;
	packet.Decode(buffer);
	
	ASSERT_EQ(packet.GetInt8Val(), -5);
	ASSERT_EQ(packet.GetInt16Val(), -1000);
	ASSERT_EQ(packet.GetInt32Val(), 50000);
	ASSERT_EQ(packet.GetInt64Val(), 9999999999LL);
	ASSERT_EQ(packet.GetUInt8Val(), 255);
	ASSERT_EQ(packet.GetUInt16Val(), 60000);
	ASSERT_EQ(packet.GetFloatVal(), 3.14f);
	ASSERT_EQ(packet.GetDoubleVal(), 2.718281828);
}