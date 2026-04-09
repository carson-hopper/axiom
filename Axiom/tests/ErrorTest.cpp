#include "TestFramework.h"

#include "Axiom/Core/Error.h"
#include "Axiom/Environment/Level/Chunk.h"

using namespace Axiom;

TEST(ErrorTest, ErrorCodeMessages) {
	auto error = make_error_code(ErrorCode::Success);
	ASSERT_TRUE(error.message() == "Success");
	
	auto error2 = make_error_code(ErrorCode::ConnectionClosed);
	ASSERT_TRUE(error2.message().find("closed") != std::string::npos);
	
	auto error3 = make_error_code(ErrorCode::BlockOutOfBounds);
	ASSERT_TRUE(error3.message().find("bounds") != std::string::npos);
}

TEST(ErrorTest, ResultSuccess) {
	Result<int> result = 42;
	ASSERT_TRUE(result.has_value());
	ASSERT_TRUE(result.value() == 42);
}

TEST(ErrorTest, ResultError) {
	auto result = MakeError<int>(ErrorCode::InvalidArgument);
	ASSERT_FALSE(result.has_value());
}

TEST(ErrorTest, ResultVoidSuccess) {
	Result<void> result{};
	ASSERT_TRUE(result.has_value());
}

TEST(ErrorTest, ResultVoidError) {
	auto result = MakeError<void>(ErrorCode::NotImplemented);
	ASSERT_FALSE(result.has_value());
}

// Integration test with Chunk
TEST(ErrorTest, ChunkSetBlockResult) {
	Chunk chunk(0, 0);
	
	// Valid position should succeed
	auto result1 = chunk.SetBlockState(0, 70, 0, 42);
	ASSERT_TRUE(result1.has_value());
	
	// Out of bounds Y should fail
	auto result2 = chunk.SetBlockState(0, -100, 0, 42);
	ASSERT_FALSE(result2.has_value());
	
	// Out of bounds Y (high) should fail
	auto result3 = chunk.SetBlockState(0, 500, 0, 42);
	ASSERT_FALSE(result3.has_value());
}