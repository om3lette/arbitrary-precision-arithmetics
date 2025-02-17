#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace test {
class Tester {
  private:
	struct Test {
		std::function<bool()> testFunction;
		std::string infoMessage;
		bool isExceptionTest;
	};
	std::string groupName;
	uint32_t testSuccess;
	uint32_t testsTotal;
	std::vector<Test> tests;

  public:
	Tester(std::string groupName);
	void registerTest(
		std::function<bool()> testFunction, const std::string &testMessage,
		bool isExceptionTest = false
	);
	bool runTests(void);
};
} // namespace test
