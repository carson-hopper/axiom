#include "TestFramework.h"

namespace Axiom::Test {

inline int TestRegistry::RunAll() const {
	std::cout << "\n========== Axiom Test Suite ==========\n" << std::endl;

	int passed = 0;
	int failed = 0;
	std::string currentSuite;

	auto startTime = std::chrono::high_resolution_clock::now();

	for (const auto& testCase : m_Tests) {
		// Print suite header when suite changes
		if (testCase.suiteName != currentSuite) {
			currentSuite = testCase.suiteName;
			std::cout << "\n[" << currentSuite << "]" << std::endl;
		}

		std::cout << "  " << testCase.testName << " ... " << std::flush;

		try {
			testCase.func();
			std::cout << "PASSED" << std::endl;
			passed++;
		} catch (const std::exception& e) {
			std::cout << "FAILED" << std::endl;
			std::cout << "    Error: " << e.what() << std::endl;
			std::cout << "    Location: " << testCase.filename << ":" << testCase.lineNumber << std::endl;
			failed++;
		} catch (...) {
			std::cout << "FAILED" << std::endl;
			std::cout << "    Error: Unknown exception" << std::endl;
			std::cout << "    Location: " << testCase.filename << ":" << testCase.lineNumber << std::endl;
			failed++;
		}
	}

	const auto endTime = std::chrono::high_resolution_clock::now();
	const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

	std::cout << "\n========== Results ==========" << std::endl;
	std::cout << "Total:  " << (passed + failed) << std::endl;
	std::cout << "Passed: " << passed << std::endl;
	std::cout << "Failed: " << failed << std::endl;
	std::cout << "Time:   " << duration.count() << "ms" << std::endl;
	std::cout << "============================\n" << std::endl;

	return failed > 0 ? 1 : 0;
}

} // namespace Axiom::Test

// Main entry point
int main() {
	return Axiom::Test::RunAllTests();
}