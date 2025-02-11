#include "Tester.hpp"
#include <iomanip>
#include <iostream>

namespace test {
Tester::Tester(std::string groupName)
	: groupName(groupName), testSuccess(0), testsTotal(0),
	  tests(std::vector<Test>()) {}

void Tester::registerTest(
	std::function<bool()> testFunction, const std::string &testMessage,
	bool isExceptionTest
) {
	tests.push_back({testFunction, testMessage, isExceptionTest});
	testsTotal++;
}

bool Tester::runTests(void) {
	std::cout << '\n'
			  << "=========== Running [" << groupName << ']'
			  << " ===========\n";
	if (tests.size() == 0) {
		std::cout << "\n\033[1;33m[WARNING]\033[0m No tests were found. Please "
					 "register some "
					 "before running.\n";
		return true;
	}
	for (int i = 0; i < tests.size(); i++) {
		Test test = tests[i];
		bool success = false;
		if (!test.isExceptionTest)
			success = test.testFunction();
		else {
			try {
				test.testFunction();
			} catch (...) {
				success = true;
			}
		}
		testSuccess += success;
		std::cout << i + 1 << ") " << test.infoMessage << ": "
				  << (success ? "\033[1;32m[PASSED]\033[0m"
							  : "\033[1;31m[FAILED]\033[0m")
				  << '\n';
	}
	float correctPercentage =
		(static_cast<float>(testSuccess) / testsTotal) * 100;
	std::cout << std::setprecision(2) << std::fixed;
	int color = 32; // Green
	if (testSuccess == 0)
		color = 31; // Red
	else if (testSuccess < testsTotal)
		color = 33; // Yellow
	std::cout << "\033[1;" << color
			  << "m\nTest results: " << std::to_string(testSuccess) << '/'
			  << std::to_string(testsTotal) << ' ' << correctPercentage
			  << "%\033[0m" << std::endl;
	return testsTotal == testSuccess;
}
} // namespace test
