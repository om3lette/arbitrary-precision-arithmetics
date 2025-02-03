#include <string>
#include <vector>
#include <cstdint>
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <ctype.h>
#include <sstream>
#include <algorithm>
#include <ostream>

class LongNumber {
    private:
        std::vector<uint32_t> chunks;
        bool sign;
        short base;
        uint32_t digitsAfterDecimal;
    
        inline int charToDigit(const char c) const {
            int digit = 0;
            if (isdigit(c)) {
                digit = static_cast<int>(c - '0');
            } else if (c >= 'A' && c <= 'Z'){
                digit = static_cast<int>(c - 'A' + 10);
            } else if (c >= 'a' && c <= 'z') {
                digit = static_cast<int>(c - 'a' + 10);
            } else {
                throw std::invalid_argument("Invalid character found. Character should match [a-zA-Z0-9]");
            }
            if (digit >= base) throw std::invalid_argument("Found digit that cannot exist in the given base");
            return digit;
        }
        
        inline char digitToChar(const int d) const {
            if (d < 10) return '0' + d;
            return 'A' + (d - 10);
        }

        // Calculate how many chunks are storing after decimal point values
        inline int getFractionChunks(void) const {
            return std::ceil(digitsAfterDecimal / (32.0 / std::log2(base)));
        }
        
        // Calculate how many digits fit in a 32-bit chunk
        inline int getDigitsPerChunk(void) const {
            return std::floor(32 / std::log2(base));
        }
    
    public:
        LongNumber():
            sign(false),
            digitsAfterDecimal(0),
            base(2),
            chunks(std::vector<uint32_t>()) {}
        ~LongNumber() = default;

        static LongNumber fromString(std::string input, uint32_t digitsAfterDecimal, int base = 2) {
            // Verify that base is correct
            // Base > 36 includes non alphabetic characters 
            if (base < 2 || base > 36) throw std::invalid_argument("Base must be in range [2; 36]!");

            LongNumber result;
            result.digitsAfterDecimal = digitsAfterDecimal;
            result.base = base;
            // Add support for float
            size_t dotPos = input.find('.');
            std::string wholePartStr = (dotPos != std::string::npos) ? input.substr(0, dotPos) : input;
            std::string fractionPartStr = (dotPos != std::string::npos) ? input.substr(dotPos + 1, input.size() - dotPos + 1) : "";

            if (wholePartStr[0] == '-') {
                result.sign = true;
                wholePartStr = wholePartStr.substr(1);
            }

            int digitsPerChunk = result.getDigitsPerChunk();
            
            // Parse fraction part before whole. So it would be stored at the start
            result.parseFractionPart(fractionPartStr, base, digitsPerChunk);
            result.parseWholePart(wholePartStr, base, digitsPerChunk);
            return result;
        }
        void parseFractionPart(std::string input, int base, int digitsPerChunk){
            for (int i = 0; i < input.size() && i < digitsAfterDecimal; i += digitsPerChunk) {
                // Dont get out of bounce
                int endIndex = std::min(static_cast<int>(input.size()), i + digitsPerChunk);
                // Do not exceed the set digitsAfterDecimal limit
                std::string strChunk = input.substr(i, std::min(endIndex - i, static_cast<int>(digitsAfterDecimal - i)));
                std::cout << "Str chunk (frac): " << strChunk << std::endl;

                uint64_t fractionValue = 0;
                uint64_t fractionScale = 1;
                
                for (int j = 0; j < strChunk.size(); j++) {
                    fractionValue = fractionValue * base + charToDigit(strChunk[j]);
                    fractionScale *= base;
                }
                fractionValue *= (1ULL << 32);
                uint32_t chunk = static_cast<uint32_t>(fractionValue / fractionScale);
                std::cout << "Str chunk value (frac): " << fractionValue << " | " << chunk << '\n' << std::endl;
                chunks.push_back(chunk);
            }
            // Add missing chunks to meet the digitsAfterDecimal quota
            chunks.resize(getFractionChunks(), 0);
        }

        void parseWholePart(std::string input, int base, int digitsPerChunk) {
            for (int i = input.size(); i > 0; i -= digitsPerChunk) {
                // Make sure that we are not getting out of bound
                int startIndex = std::max(0, i - digitsPerChunk);
                std::string strChunk = input.substr(startIndex, i - startIndex);
                std::cout << "Str chunk (whole): " << strChunk << std::endl;

                uint32_t chunkValue = 0;
                // Base ^ (str.size() - i - 1)
                uint32_t chunkMultiplier = 1;

                for (int j = strChunk.size() - 1; j > -1; j--) {
                    chunkValue += charToDigit(strChunk[j]) * chunkMultiplier;
                    chunkMultiplier *= base;
                }
                std::cout << "Str chunk value (whole): " << chunkValue << '\n' << std::endl;
                // Add assembled chunk to LongNumber
                chunks.push_back(chunkValue);
            }
            // Remove trailing zeros
            while(chunks.size() - getFractionChunks() > 1 && chunks[chunks.size() - 1] == 0) chunks.pop_back();
        }
        friend std::ostream& operator<<(std::ostream& os, const LongNumber& number) {
            std::string output;
            int fractionChunks = number.getFractionChunks();
            for (int i = number.chunks.size() - 1; i > fractionChunks - 1; i--) {
                uint32_t chunk = number.chunks[i];
                std::string chunkStr;
                while (chunk > 0) {
                    int digit = chunk % number.base;
                    chunkStr += number.digitToChar(digit);
                    chunk /= number.base;
                };
                
                // Reverse to correct order
                std::reverse(chunkStr.begin(), chunkStr.end());
                output += chunkStr;
            }

            if (output.empty()) output = "0";
            if (fractionChunks == 0) {
                os << output << std::endl;
                return os;
            }

            output += '.';
            uint64_t remainder = 0;
            uint64_t basePower = 1ULL << 32;  // 2^32
            int digitsPerChunk = number.getDigitsPerChunk();

            // Process each 32-bit chunk
            for (size_t i = 0; i < fractionChunks; i++) {
                remainder = number.chunks[i];
                // Convert the current chunk into base `base`
                // Print `digitPerChunk` digits, without exceeding the set limit `digitsAfterDecimal`
                int j = 0;
                while (j < digitsPerChunk && i * digitsPerChunk + j < number.digitsAfterDecimal) {
                    remainder *= number.base;
                    int digit = remainder / basePower;
                    remainder %= basePower;  // Keep only the fractional remainder
                    output += number.digitToChar(digit);
                    j++;
                }
            }
            os << output << std::endl;
            return os;
        }

        // Debug purposes only
        void printChunks(void) {
            int fractionChunks = getFractionChunks();
            std::cout << "Chunks (little endian): [";
            for (int i = 0; i < chunks.size(); i++) {  
                if (fractionChunks != 0 && i == fractionChunks) std::cout << " | ";
                std::cout << chunks[i];
                if (i != fractionChunks - 1 && i != chunks.size() - 1) std::cout << ", ";
            }
            std::cout << "]" << std::endl;
        }
};

int main(void) {
    LongNumber x = LongNumber::fromString("10.101100000", 9, 2);
    std::cout << "X value: " << x;
    x.printChunks();
    return 0;
}
