#ifndef LONGNUMBER_HPP
#define LONGNUMBER_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <iostream>

namespace LongArithm {
    class LongNumber {
        private:
            std::vector<uint32_t> chunks;
            bool sign;
            short base;
            uint32_t digitsAfterDecimal;

            inline int charToDigit(const char c) const;
            inline char digitToChar(const int d) const;
            inline int getFractionChunks(void) const;
            inline int getDigitsPerChunk(void) const;

            void parseFractionPart(std::string input, int base, int digitsPerChunk);
            void parseWholePart(std::string input, int base, int digitsPerChunk);

        public:
            LongNumber();
            ~LongNumber() = default;

            static LongNumber fromString(std::string input, uint32_t digitsAfterDecimal, int base);
            void printChunks(void);
            const std::string toString(void) const;
    };
    std::ostream& operator<<(std::ostream& os, const LongNumber& number);
}

#endif // LONGNUMBER_HPP
