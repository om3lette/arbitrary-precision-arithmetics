#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <ctype.h>
#include <sstream>
#include <algorithm>
#include "LongArithm.hpp"

namespace LongArithm {
    void LongNumber::allocateFraction(void) {
        chunks = std::vector<uint32_t>();
        int fracChunks = getFractionChunks();
        while (chunks.size() < fracChunks) chunks.push_back(0);
    }

    LongNumber operator""_longnum(long double number) {
        return LongNumber(number);
    }

    LongNumber::LongNumber(const std::string input, int _fractionBits) {
        fromString(input, _fractionBits);
    }
    LongNumber::LongNumber(int input, int _fractionBits) {
        fractionBits = _fractionBits;
        sign = input < 0 ? -1 : 1;
        allocateFraction();
        uint32_t chunk = 0;

        int digit = 0;
        int i = 0;
        while (input != 0) {
            digit = input % 2;
            if (digit != 0) chunk |= (1UL << i);
            input /= 2;
            i++;
        }
        chunks.push_back(chunk);
    }

    LongNumber::LongNumber(long double input, int _fractionBits) {
        sign = input < 0 ? -1 : 1;
        fractionBits = _fractionBits;
        allocateFraction();

        long double wholePart;
        long double fracPart = std::modf(input, &wholePart);
        while (wholePart >= 1) {
            chunks.push_back(static_cast<uint32_t>(std::fmod(wholePart, (1ULL << 32))));
            wholePart /= (1ULL << 32);
        }
        int i = 0;
        while (fracPart > 0) {
            fracPart *= (1ULL << 32);
            uint32_t chunk = static_cast<uint32_t>(fracPart);
            chunks[getFractionChunks() - i - 1] = chunk;
            fracPart -= chunk;
            i++;
        }
    }
            
    inline char LongNumber::digitToChar(const int d) const {
        return d + '0';
    }

    // Calculate how many chunks are storing after decimal point values
    inline int LongNumber::getFractionChunks(void) const {
        return std::ceil(static_cast<float>(fractionBits) / digitsPerChunk);
    }

    void LongNumber::parseString(const std::string& input) {
        if (input.size() == 0) return;
        // From the least significant to the most significant
        long long i = input.size() - 1;
        long long j = 0;
        char c;
        uint32_t curChunk = 0;
        while ((c = input[i]), i >= 0) {
            int indexInChunk = j % 32;
            if (indexInChunk % digitsPerChunk == 0 && j != 0) {
                chunks.push_back(curChunk);
                curChunk = 0;
            }
            if (c < '0' || c > '1') throw std::invalid_argument("Invalid character found. Character should match [01]");
            if (c == '1') curChunk |= (1 << indexInChunk); 
            i--;
            j++;
        }
        chunks.push_back(curChunk);
    }

    void LongNumber::fromString(const std::string& input, uint32_t _fractionBits) {
        fractionBits = _fractionBits;

        // Divide string into whole and fraction (after decimal point) parts
        size_t dotPos = input.find('.');
        std::string wholePartStr = (dotPos != std::string::npos) ? input.substr(0, dotPos) : input;
        std::string fractionPartStr = (dotPos != std::string::npos) ? input.substr(dotPos + 1, fmin(input.size() - dotPos + 1, fractionBits)) : "";
        // Init sign, remove from string if present
        sign = wholePartStr[0] != '-';
        if (sign == -1) wholePartStr = wholePartStr.erase(0, 1);
        
        // Parse fraction part before whole. So it would be stored at the start
        short rem = fractionPartStr.size() % 32;
        fractionPartStr.insert(fractionPartStr.size(), rem == 32 || fractionPartStr.size() == 0 ? 0 : 32 - rem, '0');
        parseString(fractionPartStr);
        while (chunks.size() < getFractionChunks()) chunks.push_back(0);
        

        rem = wholePartStr.size() % 32;
        wholePartStr.insert(0, rem == 32 ? 0 : 32 - rem, '0');
        parseString(wholePartStr);
        // Remove trailing zeros
        while(chunks.size() - getFractionChunks() > 1 && chunks[chunks.size() - 1] == 0) chunks.pop_back();
    }

    const std::string LongArithm::LongNumber::toString(void) const {
        std::string output;
        if (sign == -1) output += '-';

        uint32_t currentChunkIndex = 0;
        bool allowOut = false;
        int fractionChunks = getFractionChunks();
        for (int i = chunks.size() - 1; i >= fractionChunks; i--) {
            uint32_t curChunk = chunks[i];
            for (int j = digitsPerChunk - 1; j >= 0; j--) {
                int bit = ((curChunk & (1UL << j)) >> j);
                allowOut |= bit;
                if (allowOut) output += digitToChar(bit);
            }
        }
        if (fractionBits == 0) return output;
        output += '.';
        for (int i = fractionChunks - 1; i >= 0; i--) {
            uint32_t curChunk = chunks[i];
            for (int j = digitsPerChunk - 1; j >= 0; j--) {
                // If fraction bit is exceeded break
                if ((fractionChunks - i - 1) * digitsPerChunk + (digitsPerChunk - j) > fractionBits) return output;
                output += digitToChar((curChunk & (1UL << j)) >> j);
            }
        }
        return output;
    }

    std::ostream& operator<<(std::ostream& os, const LongNumber& number) {
        os << number.toString();
        return os;
    }

    // Debug purposes only
    void LongNumber::printChunks(void) const {
        int fractionChunks = getFractionChunks();
        std::cout << "Chunks (little endian): [";
        for (int i = 0; i < chunks.size(); i++) {  
            if (fractionChunks != 0 && i == fractionChunks) std::cout << " | ";
            std::cout << chunks[i];
            if (i != fractionChunks - 1 && i != chunks.size() - 1) std::cout << ", ";
        }
        std::cout << "] | " << "Precision: " << fractionBits << std::endl;
    }
}
