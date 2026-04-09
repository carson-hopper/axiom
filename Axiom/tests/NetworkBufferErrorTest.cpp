#include "TestFramework.h"

#include "Axiom/Network/NetworkBuffer.h"
#include "Axiom/Core/Error.h"

using namespace Axiom;

TEST(NetworkBufferErrorTest, TryReadByteFromEmptyBuffer) {
	NetworkBuffer buffer;
	
	auto result = buffer.TryReadByte();
	ASSERT_FALSE(result.has_value());
}

TEST(NetworkBufferErrorTest, TryReadBooleanFromEmptyBuffer) {
	NetworkBuffer buffer;
	
	auto result = buffer.TryReadBoolean();
	ASSERT_FALSE(result.has_value());
}

TEST(NetworkBufferErrorTest, TryReadByteSuccess) {
	NetworkBuffer buffer;
	buffer.WriteByte(42);
	
	auto result = buffer.TryReadByte();
	ASSERT_TRUE(result.has_value());
	ASSERT_EQ(*result, 42);
}

TEST(NetworkBufferErrorTest, TryReadVarIntSuccess) {
	NetworkBuffer buffer;
	buffer.WriteVarInt(12345);
	
	auto result = buffer.TryReadVarInt();
	ASSERT_TRUE(result.has_value());
	ASSERT_EQ(*result, 12345);
}

TEST(NetworkBufferErrorTest, TryReadVarIntPartialData) {
	NetworkBuffer buffer;
	// Write incomplete VarInt (missing continuation byte)
	buffer.WriteByte(0x80);  // This indicates more bytes follow
	
	auto result = buffer.TryReadVarInt();
	ASSERT_FALSE(result.has_value());
}

TEST(NetworkBufferErrorTest, TryReadVarLongSuccess) {
	NetworkBuffer buffer;
	buffer.WriteVarLong(9999999999LL);
	
	auto result = buffer.TryReadVarLong();
	ASSERT_TRUE(result.has_value());
	ASSERT_EQ(*result, 9999999999LL);
}

TEST(NetworkBufferErrorTest, TryReadStringSuccess) {
	NetworkBuffer buffer;
	buffer.WriteString("Hello, World!");
	
	auto result = buffer.TryReadString();
	ASSERT_TRUE(result.has_value());
	ASSERT_EQ(*result, "Hello, World!");
}

TEST(NetworkBufferErrorTest, TryReadStringEmpty) {
	NetworkBuffer buffer;
	buffer.WriteString("");
	
	auto result = buffer.TryReadString();
	ASSERT_TRUE(result.has_value());
	ASSERT_EQ(*result, "");
}

TEST(NetworkBufferErrorTest, TryReadStringLengthError) {
	NetworkBuffer buffer;
	// Write a length that's larger than available data
	buffer.WriteVarInt(100);
	buffer.WriteByte('a');
	
	auto result = buffer.TryReadString();
	ASSERT_FALSE(result.has_value());
}

TEST(NetworkBufferErrorTest, TryReadBytesSuccess) {
	NetworkBuffer buffer;
	std::vector<uint8_t> data = {1, 2, 3, 4, 5};
	buffer.WriteBytes(data);
	
	auto result = buffer.TryReadBytes(5);
	ASSERT_TRUE(result.has_value());
	ASSERT_EQ(result->size(), 5);
	ASSERT_EQ((*result)[0], 1);
	ASSERT_EQ((*result)[4], 5);
}

TEST(NetworkBufferErrorTest, TryReadBytesInsufficientData) {
	NetworkBuffer buffer;
	buffer.WriteByte(1);
	buffer.WriteByte(2);
	
	auto result = buffer.TryReadBytes(5);
	ASSERT_FALSE(result.has_value());
}

TEST(NetworkBufferErrorTest, CanReadCheck) {
	NetworkBuffer buffer;
	buffer.WriteByte(1);
	buffer.WriteByte(2);
	buffer.WriteByte(3);
	
	ASSERT_TRUE(buffer.CanRead(1));
	ASSERT_TRUE(buffer.CanRead(2));
	ASSERT_TRUE(buffer.CanRead(3));
	ASSERT_FALSE(buffer.CanRead(4));
	ASSERT_FALSE(buffer.CanRead(100));
}

TEST(NetworkBufferErrorTest, SequentialReadsWithCanRead) {
	NetworkBuffer buffer;
	for (int i = 0; i < 10; i++) {
		buffer.WriteByte(static_cast<uint8_t>(i));
	}
	
	// Read all bytes one by one
	for (int i = 0; i < 10; i++) {
		ASSERT_TRUE(buffer.CanRead(1));
		auto result = buffer.TryReadByte();
		ASSERT_TRUE(result.has_value());
		ASSERT_EQ(*result, i);
	}
	
	// Buffer should be empty now
	ASSERT_FALSE(buffer.CanRead(1));
}

TEST(NetworkBufferErrorTest, ErrorCodePropagation) {
	NetworkBuffer buffer;
	
	auto result = buffer.TryReadByte();
	if (!result) {
		// Result should contain an error
		std::error_code ec = result.error();
		// Just verify we can access the error
		ASSERT_FALSE(ec.message().empty());
	}
}

TEST(NetworkBufferErrorTest, MixedOperations) {
	NetworkBuffer buffer;
	
	// Write various data types
	buffer.WriteVarInt(100);
	buffer.WriteString("Test");
	buffer.WriteBoolean(true);
	
	// Read with Try methods
	auto intResult = buffer.TryReadVarInt();
	ASSERT_TRUE(intResult.has_value());
	ASSERT_EQ(*intResult, 100);
	
	auto stringResult = buffer.TryReadString();
	ASSERT_TRUE(stringResult.has_value());
	ASSERT_EQ(*stringResult, "Test");
	
	auto boolResult = buffer.TryReadBoolean();
	ASSERT_TRUE(boolResult.has_value());
	ASSERT_EQ(*boolResult, true);
	
	// Buffer should be empty
	ASSERT_FALSE(buffer.CanRead(1));
}

TEST(NetworkBufferErrorTest, BufferClearAndReuse) {
	NetworkBuffer buffer;
	buffer.WriteVarInt(42);
	buffer.ReaderIndex(0);
	
	// Read the value
	auto result1 = buffer.TryReadVarInt();
	ASSERT_TRUE(result1.has_value());
	ASSERT_EQ(*result1, 42);
	
	// Clear and reuse
	buffer.Clear();
	buffer.WriteVarInt(100);
	buffer.ReaderIndex(0);
	
	auto result2 = buffer.TryReadVarInt();
	ASSERT_TRUE(result2.has_value());
	ASSERT_EQ(*result2, 100);
}
