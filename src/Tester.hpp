#pragma once

#include <string>
#include <functional>
#include <vector>
#include <optional>

namespace test {
    class Tester {
        private:
            struct Test {
                std::function<bool()> testFunction;
                std::string infoMessage;
                bool isExceptionTest;
            };
            std::vector<Test> tests;
            std::string groupName;
            int testSuccess;
            int testsTotal;
        public:
            Tester(std::string groupName);
            void registerTest(std::function<bool()> testFunction, const std::string& testMessage, bool isExceptionTest  = false);
            bool runTests(void);
    };
}
