#include "TestFramework.h"

#include "Axiom/Network/NetworkBuffer.h"
#include "Axiom/Network/Packet/PacketMacros.h"

using namespace Axiom;

// Test packet using macros for round-trip testing
PACKET_DECL_BEGIN(RoundTripTestPacket, Play, 0x01)
	PACKET_FIELD_INT32(TestInt)
	PACKET_FIELD_STRING(TestString)
	PACKET_FIELD_BOOL(TestBool)
PACKET_DECL_END()

PACKET_DECODE_BEGIN(RoundTripTestPacket)
	READ_VARINT(m_TestInt);
	READ_STRING(m_TestString);
	READ_BOOL(m_TestBool);
PACKET_DECODE_END()

PACKET_INSTANTIATE(RoundTripTestPacket, 775)

// Clientbound version for encode/decode testing
CLIENTBOUND_PACKET_DECL_BEGIN(ClientboundRoundTripPacket, Play, 0x02)
	PACKET_FIELD_INT32(TestInt)
	PACKET_FIELD_STRING(TestString)
	PACKET_FIELD_BOOL(TestBool)
CLIENTBOUND_PACKET_DECL_END()

CLIENTBOUND_PACKET_ENCODE_BEGIN(ClientboundRoundTripPacket)
	WRITE_VARINT(m_TestInt);
	WRITE_STRING(m_TestString);
	WRITE_BOOL(m_TestBool);
CLIENTBOUND_PACKET_ENCODE_END()

CLIENTBOUND_PACKET_INSTANTIATE(ClientboundRoundTripPacket, 775)

// Test coordinate packet
PACKET_DECL_BEGIN(CoordRoundTripPacket, Play, 0x03)
	PACKET_FIELD_COORDS(Position)
PACKET_DECL_END()

PACKET_DECODE_BEGIN(CoordRoundTripPacket)
	READ_BLOCK_POS(m_PositionX, m_PositionY, m_PositionZ);
PACKET_DECODE_END()

PACKET_INSTANTIATE(CoordRoundTripPacket, 775)

TEST(PacketRoundTripTest, BasicRoundTrip) {
	// Create and populate a packet
	ClientboundRoundTripPacket<775> original;
	original.SetTestInt(12345);
	original.SetTestString("Hello, Minecraft!");
	original.SetTestBool(true);
	
	// Encode to buffer
	NetworkBuffer buffer;
	original.Encode(buffer);
	
	// Verify buffer has data
	ASSERT_TRUE(buffer.Size() > 0);
	
	// Reset reader index
	buffer.ReaderIndex(0);
	
	// Decode into a new packet
	RoundTripTestPacket<775> decoded;
	decoded.Decode(buffer);
	
	// Verify values match
	ASSERT_EQ(original.GetTestInt(), decoded.GetTestInt());
	ASSERT_EQ(original.GetTestString(), decoded.GetTestString());
	ASSERT_EQ(original.IsTestBool(), decoded.IsTestBool());
}

TEST(PacketRoundTripTest, EmptyString) {
	ClientboundRoundTripPacket<775> original;
	original.SetTestInt(42);
	original.SetTestString("");
	original.SetTestBool(false);
	
	NetworkBuffer buffer;
	original.Encode(buffer);
	buffer.ReaderIndex(0);
	
	RoundTripTestPacket<775> decoded;
	decoded.Decode(buffer);
	
	ASSERT_EQ(decoded.GetTestInt(), 42);
	ASSERT_EQ(decoded.GetTestString(), "");
	ASSERT_EQ(decoded.IsTestBool(), false);
}

TEST(PacketRoundTripTest, LargeValues) {
	ClientboundRoundTripPacket<775> original;
	original.SetTestInt(2147483647);  // Max int32
	original.SetTestString("A very long string with lots of characters that tests the varint length encoding properly");
	original.SetTestBool(true);
	
	NetworkBuffer buffer;
	original.Encode(buffer);
	buffer.ReaderIndex(0);
	
	RoundTripTestPacket<775> decoded;
	decoded.Decode(buffer);
	
	ASSERT_EQ(decoded.GetTestInt(), 2147483647);
	ASSERT_EQ(decoded.GetTestString().length(), 93);
	ASSERT_EQ(decoded.IsTestBool(), true);
}

TEST(PacketRoundTripTest, NegativeValues) {
	ClientboundRoundTripPacket<775> original;
	original.SetTestInt(-12345);
	original.SetTestString("Negative test");
	original.SetTestBool(false);
	
	NetworkBuffer buffer;
	original.Encode(buffer);
	buffer.ReaderIndex(0);
	
	RoundTripTestPacket<775> decoded;
	decoded.Decode(buffer);
	
	ASSERT_EQ(decoded.GetTestInt(), -12345);
}

TEST(PacketRoundTripTest, CoordinateRoundTrip) {
	// Create buffer with coordinate data
	NetworkBuffer buffer;
	buffer.WriteBlockPosition(100, 70, -50);
	buffer.ReaderIndex(0);
	
	CoordRoundTripPacket<775> decoded;
	decoded.Decode(buffer);
	
	ASSERT_EQ(decoded.GetPositionX(), 100);
	ASSERT_EQ(decoded.GetPositionY(), 70);
	ASSERT_EQ(decoded.GetPositionZ(), -50);
}

TEST(PacketRoundTripTest, MultiplePacketsInBuffer) {
	NetworkBuffer buffer;
	
	// Encode multiple packets
	for (int i = 0; i < 5; i++) {
		ClientboundRoundTripPacket<775> packet;
		packet.SetTestInt(i);
		packet.SetTestString("Packet " + std::to_string(i));
		packet.SetTestBool(i % 2 == 0);
		packet.Encode(buffer);
	}
	
	// Decode all packets
	buffer.ReaderIndex(0);
	for (int i = 0; i < 5; i++) {
		RoundTripTestPacket<775> decoded;
		decoded.Decode(buffer);
		
		ASSERT_EQ(decoded.GetTestInt(), i);
		ASSERT_EQ(decoded.GetTestString(), "Packet " + std::to_string(i));
		ASSERT_EQ(decoded.IsTestBool(), i % 2 == 0);
	}
}

TEST(PacketRoundTripTest, UnicodeStrings) {
	ClientboundRoundTripPacket<775> original;
	original.SetTestInt(100);
	original.SetTestString("Hello 世界! 🎉");  // "Hello World!" in Chinese + emoji
	original.SetTestBool(true);
	
	NetworkBuffer buffer;
	original.Encode(buffer);
	buffer.ReaderIndex(0);
	
	RoundTripTestPacket<775> decoded;
	decoded.Decode(buffer);
	
	ASSERT_EQ(decoded.GetTestInt(), 100);
	ASSERT_EQ(decoded.GetTestString(), "Hello 世界! 🎉");
	ASSERT_EQ(decoded.IsTestBool(), true);
}

TEST(PacketRoundTripTest, PacketConstants) {
	// Verify packet constants are correct
	ASSERT_EQ(RoundTripTestPacket<775>::PacketId, 0x01);
	ASSERT_EQ(ClientboundRoundTripPacket<775>::PacketId, 0x02);
	ASSERT_EQ(CoordRoundTripPacket<775>::PacketId, 0x03);
	
	ASSERT_EQ(RoundTripTestPacket<775>::PacketState, ConnectionState::Play);
	ASSERT_EQ(ClientboundRoundTripPacket<775>::PacketState, ConnectionState::Play);
}
