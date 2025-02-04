#include "LongArithm.hpp"
#include "Tester.hpp"

using namespace LongArithm;

int main(void) {
    // TODO: Only support base 2 numbers
    bool success = true;

    test::Tester tester("Cout tests");
    LongNumber x = LongNumber::fromString("10.10111", 3, 2);
    LongNumber y = LongNumber::fromString("10.11111", 3, 2);
    tester.registerTest(
        [x]() {return x.toString() == "10.101";},
        "10.10111 -> 10.101"
    );
    tester.registerTest(
        [y]() {return y.toString() == "10.111";},
        "10.11111 -> 10.111"
    );
    
    success &= tester.runTests();
    
    test::Tester tester2("Exception tests");
    tester2.registerTest(
        []() {LongNumber::fromString("20.1", 3, 2); return true;},
        "Impossible digit (2) in whole part",
        true
    );
    tester2.registerTest(
        []() {LongNumber::fromString("10.2", 3, 2); return true;},
        "Impossible digit (2) in fraction part",
        true
    );
    success &= tester2.runTests();
    
    if (!success) throw std::logic_error("Some tests failed!");
    return 0;
}
