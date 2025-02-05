#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <iostream>

#define digitsPerChunk 32

namespace LongArithm {
    class LongNumber {
        private:
            std::vector<uint32_t> chunks;
            short sign;
            uint32_t fractionBits;

            void allocateFraction(void);
            inline char digitToChar(const int d) const;
            inline int getFractionChunks(void) const;
            void parseString(const std::string& input);
            void fromString(const std::string& input, uint32_t fractionalBits);

        public:
            LongNumber(int input, int _fractionBits = 0);
            LongNumber(long double input, int _fractionBits = 96);

            LongNumber(const std::string input, int _fractionBits);
            ~LongNumber() = default;

            void printChunks(void) const;
            const std::string toString(void) const;
    };
    LongNumber operator""_longnum(long double value);
    std::ostream& operator<<(std::ostream& os, const LongNumber& number);
}
