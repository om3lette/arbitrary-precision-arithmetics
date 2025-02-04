#include "Tester.hpp"
#include <iostream>
#include <iomanip>

namespace test {
    Tester::Tester(std::string groupName):
        groupName(groupName),
        testSuccess(0),
        testsTotal(0),
        tests(std::vector<Test>()) {}
    
    void Tester::registerTest(std::function<bool()> testFunction, const std::string& testMessage, bool isExceptionTest) {
        tests.push_back({testFunction, testMessage, isExceptionTest});
        testsTotal++;
    }

    bool Tester::runTests(void) {
        std::cout << '\n' << "=========== Running [" << groupName << ']' << " ===========\n";
        if (tests.size() == 0) {
            std::cout << "[WARNING] No tests were found. Please register some them before running.\n";
            return false;
        }
        for (int i = 0; i < tests.size(); i++) {
            Test test = tests[i];
            bool success = false;
            if (!test.isExceptionTest) success = test.testFunction();
            else {
                try {
                    test.testFunction();
                } catch(...) {
                    success = true;
                }
            }
            testSuccess += success;
            std::cout << i + 1 << ") "
                << test.infoMessage << ": " 
                << (success ? "[PASSED]" : "[FAILED]")
                << '\n';
        }
        float correctPercentage = (static_cast<float>(testSuccess) / testsTotal) * 100;
        std::cout << std::setprecision(2) << std::fixed;
        std::cout << "\nTest results: "
                << std::to_string(testSuccess) << '/' << std::to_string(testsTotal) << ' '
                << correctPercentage << "%"
                << std::endl;
        return testsTotal == testSuccess;
    }
}
