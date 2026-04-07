#pragma once

#include <functional>
#include <string>
#include <vector>
#include <chrono>
#include <iostream>
#include <stdexcept>

namespace Axiom::Test {

/**
 * Simple header-only testing framework for Axiom.
 *
 * Usage:
 * @code
 * TEST(ChunkTest, BasicOperations) {
 *     Chunk chunk(0, 0);
 *     ASSERT_EQ(chunk.GetBlockState(0, 0, 0), 0);
 * }
 * @endcode
 */

struct TestCase {
	std::string suiteName;
	std::string testName;
	std::function<void()> func;
	std::string filename;
	int lineNumber;
};

class TestRegistry {
public:
	static TestRegistry& Instance() {
		static TestRegistry instance;
		return instance;
	}

	void Register(TestCase testCase) {
		m_Tests.push_back(std::move(testCase));
	}

	int RunAll()const;

private:
	std::vector<TestCase> m_Tests;
};

struct TestRegistrar {
	explicit TestRegistrar(TestCase testCase) {
		TestRegistry::Instance().Register(std::move(testCase));
	}
};

// Assertion macros
#define TEST_ASSERT(condition) \
	if (!(condition)) { \
		throw std::runtime_error("Assertion failed: " #condition); \
	}

#define ASSERT_EQ(expected, actual) \
	if ((expected) != (actual)) { \
		throw std::runtime_error("Expected " + std::to_string(expected) + \
			" but got " + std::to_string(actual)); \
	}

#define ASSERT_NE(expected, actual) \
	if ((expected) == (actual)) { \
		throw std::runtime_error("Expected values to differ, but both were " + \
			std::to_string(expected)); \
	}

#define ASSERT_TRUE(condition) \
	if (!(condition)) { \
		throw std::runtime_error("Expected true but got false: " #condition); \
	}

#define ASSERT_FALSE(condition) \
	if (condition) { \
		throw std::runtime_error("Expected false but got true: " #condition); \
	}

#define ASSERT_NULL(ptr) \
	if ((ptr) != nullptr) { \
		throw std::runtime_error("Expected null pointer but got non-null"); \
	}

#define ASSERT_NOT_NULL(ptr) \
	if ((ptr) == nullptr) { \
		throw std::runtime_error("Expected non-null pointer but got null"); \
	}

#define ASSERT_THROWS(expression, exceptionType) \
	{ \
		bool caught = false; \
		try { \
			expression; \
		} catch (const exceptionType&) { \
			caught = true; \
		} catch (...) { \
			throw std::runtime_error("Expected " #exceptionType " but got different exception"); \
		} \
		if (!caught) { \
			throw std::runtime_error("Expected " #exceptionType " but no exception thrown"); \
		} \
	}

// Test registration macro
#define TEST(suiteName, testName) \
	void Test_##suiteName##_##testName(); \
	static Axiom::Test::TestRegistrar registrar_##suiteName##_##testName( \
		{Axiom::Test::TestCase{#suiteName, #testName, Test_##suiteName##_##testName, __FILE__, __LINE__}}); \
	void Test_##suiteName##_##testName()

// Main entry point for tests
inline int RunAllTests() {
	return TestRegistry::Instance().RunAll();
}

} // namespace Axiom::Test