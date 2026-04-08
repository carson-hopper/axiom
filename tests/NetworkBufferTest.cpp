#include "TestFramework.h"

#include "Axiom/Network/NetworkBuffer.h"

using namespace Axiom;

TEST(NetworkBufferTest, EmptyBuffer) {
	NetworkBuffer buffer;
	ASSERT_EQ(buffer.Size(), 0u);
	ASSERT_TRUE(buffer.Data().empty());
}

TEST(NetworkBufferTest, WriteAndReadByte) {
	NetworkBuffer buffer;
	buffer.WriteByte(42);

	ASSERT_EQ(buffer.Size(), 1u);
	ASSERT_EQ(buffer.ReadByte(), 42);
}

TEST(NetworkBufferTest, WriteAndReadBoolean) {
	NetworkBuffer buffer;
	buffer.WriteBoolean(true);
	buffer.WriteBoolean(false);

	ASSERT_EQ(buffer.ReadBoolean(), true);
	ASSERT_EQ(buffer.ReadBoolean(), false);
}

TEST(NetworkBufferTest, WriteAndReadShort) {
	NetworkBuffer buffer;
	buffer.WriteShort(12345);
	buffer.WriteShort(-1);

	ASSERT_EQ(buffer.ReadShort(), 12345);
	ASSERT_EQ(buffer.ReadShort(), -1);
}

TEST(NetworkBufferTest, WriteAndReadInt) {
	NetworkBuffer buffer;
	buffer.WriteInt(123456789);
	buffer.WriteInt(-987654321);

	ASSERT_EQ(buffer.ReadInt(), 123456789);
	ASSERT_EQ(buffer.ReadInt(), -987654321);
}

TEST(NetworkBufferTest, WriteAndReadLong) {
	NetworkBuffer buffer;
	buffer.WriteLong(1234567890123456789LL);
	buffer.WriteLong(-999999999999999999LL);

	ASSERT_EQ(buffer.ReadLong(), 1234567890123456789LL);
	ASSERT_EQ(buffer.ReadLong(), -999999999999999999LL);
}

TEST(NetworkBufferTest, WriteAndReadFloat) {
	NetworkBuffer buffer;
	buffer.WriteFloat(3.14159f);
	buffer.WriteFloat(-2.5f);

	ASSERT_EQ(buffer.ReadFloat(), 3.14159f);
	ASSERT_EQ(buffer.ReadFloat(), -2.5f);
}

TEST(NetworkBufferTest, WriteAndReadDouble) {
	NetworkBuffer buffer;
	buffer.WriteDouble(2.718281828459045);
	buffer.WriteDouble(-1.4142135623730951);

	ASSERT_EQ(buffer.ReadDouble(), 2.718281828459045);
	ASSERT_EQ(buffer.ReadDouble(), -1.4142135623730951);
}

TEST(NetworkBufferTest, WriteAndReadVarInt) {
	NetworkBuffer buffer;
	buffer.WriteVarInt(0);
	buffer.WriteVarInt(127);
	buffer.WriteVarInt(128);
	buffer.WriteVarInt(255);
	buffer.WriteVarInt(25565);
	buffer.WriteVarInt(2097151);
	buffer.WriteVarInt(-1);

	ASSERT_EQ(buffer.ReadVarInt(), 0);
	ASSERT_EQ(buffer.ReadVarInt(), 127);
	ASSERT_EQ(buffer.ReadVarInt(), 128);
	ASSERT_EQ(buffer.ReadVarInt(), 255);
	ASSERT_EQ(buffer.ReadVarInt(), 25565);
	ASSERT_EQ(buffer.ReadVarInt(), 2097151);
	ASSERT_EQ(buffer.ReadVarInt(), -1);
}

TEST(NetworkBufferTest, WriteAndReadString) {
	NetworkBuffer buffer;
	buffer.WriteString("Hello, World!");
	buffer.WriteString("");
	buffer.WriteString("Test with spaces and numbers 123");

	ASSERT_EQ(buffer.ReadString(), "Hello, World!");
	ASSERT_EQ(buffer.ReadString(), "");
	ASSERT_EQ(buffer.ReadString(), "Test with spaces and numbers 123");
}

TEST(NetworkBufferTest, WriteAndReadBytes) {
	NetworkBuffer buffer;
	std::vector<uint8_t> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	buffer.WriteBytes(data);

	auto result = buffer.ReadBytes(10);
	ASSERT_EQ(result.size(), 10u);
	for (size_t i = 0; i < 10; i++) {
		ASSERT_EQ(result[i], data[i]);
	}
}

TEST(NetworkBufferTest, VarIntSize) {
	ASSERT_EQ(NetworkBuffer::VarIntSize(0), 1);
	ASSERT_EQ(NetworkBuffer::VarIntSize(127), 1);
	ASSERT_EQ(NetworkBuffer::VarIntSize(128), 2);
	ASSERT_EQ(NetworkBuffer::VarIntSize(16383), 2);
	ASSERT_EQ(NetworkBuffer::VarIntSize(16384), 3);
}

TEST(NetworkBufferTest, Clear) {
	NetworkBuffer buffer;
	buffer.WriteInt(12345);
	ASSERT_EQ(buffer.Size(), 4u);

	buffer.Clear();
	ASSERT_EQ(buffer.Size(), 0u);
}

TEST(NetworkBufferTest, ConstructorFromVector) {
	std::vector<uint8_t> data = {0, 1, 2, 3, 4};
	NetworkBuffer buffer(std::move(data));

	ASSERT_EQ(buffer.Size(), 5u);
	for (int i = 0; i < 5; i++) {
		ASSERT_EQ(buffer.ReadByte(), i);
	}
}