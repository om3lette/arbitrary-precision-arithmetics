#include <ctype.h>
#include <stdlib.h>

#include <algorithm>
#include <cmath>
#include <compare>
#include <iostream>
#include <sstream>

#include "LongArithm.hpp"

namespace LongArithm {

// *BITWISE OPERATIONS*

void LongNumber::setBit(int index) {
	int chunkIndex = index / digitsPerChunk;
	int bitIndex = index % digitsPerChunk;
	if (chunkIndex >= chunks.size()) {
		// Extend vector if needed
		chunks.resize(chunkIndex + 1, 0);
	}
	chunks[chunkIndex] |= (1U << bitIndex);
}

bool LongNumber::getBit(int index) const {
	int chunkIndex = index / digitsPerChunk;
	int bitIndex = index % digitsPerChunk;
	if (chunkIndex >= chunks.size()) return false;
	return (chunks[chunkIndex] >> bitIndex) & 1;
}

// *MEMORY UTILS*

// Allocates memory for `getFractionChunks()` chunks
// 1) Is usually called on an empty vector during initialization
// 2) Makes sure that there are at least `getFractionChunks()` chunks
void LongNumber::allocateFraction(void) {
	int fracChunks = getFractionChunks();
	while (chunks.size() < fracChunks) chunks.push_back(0);
}
// Removes leading zeros
// Will remove rightmost zeros in `chunks` as number is stored in little endian
void LongNumber::truncateWholePart(void) {
	while (chunks.size() - getFractionChunks() > 0 && chunks.back() == 0)
		chunks.pop_back();
}

// *CONVERSION UTILS*

// Converts digit to corresponding chat by adding `'0'`
inline char LongNumber::digitToChar(const int d) const { return d + '0'; }
// Calculates how many chunks are storing number's fraction part
inline u_int32_t LongNumber::getFractionChunks(void) const {
	return std::ceil(static_cast<long double>(fractionBits) / digitsPerChunk);
}

// *CREATING FROM STRING*
// Helper for `fromBinaryString`
// Takes binary string not containing `.` symbol and inserts conversion result into `chunks`
// As a result fraction must be handled first, otherwise chunk order will be ruined
void LongNumber::convertBinaryString(const std::string &input) {
	if (input.size() == 0) return;
	// From the least significant to the most significant
	long long i = input.size() - 1;
	long long j = 0;
	char c;
	uint32_t curChunk = 0;
	while ((c = input[i]), i >= 0) {
		int indexInChunk = j % digitsPerChunk;
		if (indexInChunk % digitsPerChunk == 0 && j != 0) {
			chunks.push_back(curChunk);
			curChunk = 0;
		}
		if (c < '0' || c > '1')
			throw std::invalid_argument(
				"Invalid character found. Character should match [01]"
			);
		if (c == '1') curChunk |= (1 << indexInChunk);
		i--;
		j++;
	}
	chunks.push_back(curChunk);
}
// Initializes `sign`, `fractionBits`, `chunks` from a given binary string
// Throws `std::invalid argument` if digits outside of base's scope are present
void LongNumber::fromBinaryString(
	const std::string &input, uint32_t _fractionBits
) {
	fractionBits = _fractionBits;

	// Divide string into whole and fraction (after decimal point) parts
	size_t dotPos = input.find('.');
	std::string wholePartStr =
		(dotPos != std::string::npos) ? input.substr(0, dotPos) : input;
	std::string fractionPartStr =
		(dotPos != std::string::npos)
			? input.substr(
				  dotPos + 1, fmin(input.size() - dotPos + 1, fractionBits)
			  )
			: "";
	// Init sign, remove from string if present
	sign = wholePartStr[0] != '-';
	if (sign == -1) wholePartStr = wholePartStr.erase(0, 1);

	// Parse fraction part before whole. So it would be stored at the start
	short rem = fractionPartStr.size() % digitsPerChunk;
	fractionPartStr.insert(
		fractionPartStr.size(),
		rem == digitsPerChunk || fractionPartStr.size() == 0
			? 0
			: digitsPerChunk - rem,
		'0'
	);
	convertBinaryString(fractionPartStr);
	while (chunks.size() < getFractionChunks()) chunks.push_back(0);

	rem = wholePartStr.size() % digitsPerChunk;
	wholePartStr.insert(
		0, rem == digitsPerChunk ? 0 : digitsPerChunk - rem, '0'
	);
	convertBinaryString(wholePartStr);
	// Remove trailing zeros
	truncateWholePart();
}

// *USER DEFINED LITERALS*

LongNumber operator""_longnum(long double number) { return LongNumber(number); }

// *CONSTRUCTORS*

LongNumber::LongNumber() {
	// Default to 0.0L
	sign = 1;
	fractionBits = 96;
	allocateFraction();
}
LongNumber::LongNumber(long double input, uint32_t _fractionBits) {
	sign = input < 0 ? -1 : 1;
	input = abs(input);
	fractionBits = _fractionBits;
	allocateFraction();

	if (input == 0) return;

	long double wholePart;
	long double fracPart = std::modf(input, &wholePart);
	while (wholePart >= 1) {
		chunks.push_back(static_cast<uint32_t>(
			std::fmod(wholePart, (1ULL << digitsPerChunk))
		));
		wholePart /= (1ULL << digitsPerChunk);
	}
	int i = 0;
	int fracChunks = getFractionChunks();
	while (fracPart != 0 && i < fracChunks) {
		fracPart *= (1ULL << digitsPerChunk);
		uint32_t chunk = static_cast<uint32_t>(fracPart);
		chunks[fracChunks - i - 1] = chunk;
		fracPart -= chunk;
		i++;
	}
}
LongNumber::LongNumber(const std::string input, uint32_t _fractionBits) {
	fromBinaryString(input, _fractionBits);
}

// *PRECISION HANDLERS*

// Updates `fractionBits`
// Resizes `chunks` to match new precision
void LongNumber::setPrecision(uint32_t _precision) {
	uint32_t oldFracChunks = getFractionChunks();
	fractionBits = _precision;

	int chunkDif = getFractionChunks() - oldFracChunks;
	if (chunkDif == 0) return;

	if (chunkDif > 0)
		chunks.insert(chunks.begin(), chunkDif, 0);
	else
		// chunkDif < 0 => chunks.begin() - chunkDif (not +)
		chunks.erase(chunks.begin(), chunks.begin() - chunkDif);
}

// Calls `setPrecision` and returns the number
LongNumber LongNumber::withPrecision(uint32_t precision) {
	(*this).setPrecision(precision);
	return *this;
};

// Returns `chunks[index]` with its value adjusted for precision
// Throws `std::out_of_range` if index is incorrect
uint32_t LongNumber::getChunk(uint32_t index) const {
	if (index > chunks.size())
		throw std::out_of_range("Chunk index out of range");
	if (index != 0 || (fractionBits % digitsPerChunk) == 0) {
		return chunks[index];
	}
	uint32_t mask = (1UL << (fractionBits % digitsPerChunk)) - 1;
	uint8_t shift = digitsPerChunk - (fractionBits % digitsPerChunk);
	return chunks[index] & (mask << shift);
}

// *MATH UTILS*

LongNumber LongNumber::pow(uint32_t power) const {
	if (power == 1) return *this;
	// Result has the same precision
	LongNumber result(1, fractionBits);
	if (power == 0) return result;
	LongNumber accumulator = *this;

	// Minimize iterations by leveraging the closest power of 2
	while (power) {
		if (power & 1) result *= accumulator;
		accumulator *= accumulator;
		power >>= 1;
	}
	return result;
}

// Newton-Raphson method for calculating square root
LongNumber LongNumber::sqrt(void) const {
	if (sign == -1)
		throw std::invalid_argument(
			"Failed to calculate square root: number is negative"
		);
	// If integer make room for fraction part to increase accuracy
	LongNumber normalisedThis = *this;
	if (normalisedThis.fractionBits < 96) normalisedThis.setPrecision(96);

	LongNumber guess = LongNumber(1.0L, normalisedThis.fractionBits);
	LongNumber prevGuess;

	while (guess != prevGuess) {
		prevGuess = guess;
		guess += normalisedThis / guess; // guess + S / guess
		guess >>= 1;					 // guess / 2
	};

	guess.truncateWholePart();
	guess.setPrecision(fractionBits);
	return guess;
}

// *OUTPUT UTILS*

// Outputs to console `chunks` vector (stored in little endian)
// Followed by `fractionBits` and `getFractionChunks()`
void LongNumber::printChunks(void) const {
	int fractionChunks = getFractionChunks();
	std::cout << "Chunks (little endian): [";
	if (getFractionChunks() == 0) std::cout << "-";
	for (int i = 0; i < chunks.size(); i++) {
		if (fractionChunks != 0 && i == fractionChunks) std::cout << " | ";
		std::cout << chunks[i];
		if (i != fractionChunks - 1 && i != chunks.size() - 1)
			std::cout << ", ";
	}
	if (chunks.size() == getFractionChunks()) std::cout << " | -";
	std::cout << "] | " << "Precision: " << fractionBits
			  << ", Fraction chunks: " << getFractionChunks() << std::endl;
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
	if (fractionBits == 0) {
		if (output.size() == 0) output = "0";
		return output;
	}
	output += '.';
	for (int i = fractionChunks - 1; i >= 0; i--) {
		uint32_t curChunk = chunks[i];
		for (int j = digitsPerChunk - 1; j >= 0; j--) {
			// If fraction bit is exceeded break
			if ((fractionChunks - i - 1) * digitsPerChunk +
					(digitsPerChunk - j) >
				fractionBits)
				return output;
			output += digitToChar((curChunk & (1UL << j)) >> j);
		}
	}
	return output;
}

// *OPERATORS*

std::strong_ordering LongNumber::operator<=>(const LongNumber &other) const {
	// Compare signs
	if (sign < other.sign) return std::strong_ordering::less;
	if (sign > other.sign) return std::strong_ordering::greater;

	// If both numbers are zero, they are equal
	if (chunks.empty() && other.chunks.empty())
		return std::strong_ordering::equal;

	size_t wholeSizeThis = chunks.size() - getFractionChunks();
	size_t wholeSizeOther = other.chunks.size() - other.getFractionChunks();

	// For negative numbers the comparison sign needs to be "reversed"
	if (wholeSizeThis != wholeSizeOther)
		return (sign == 1) ? (wholeSizeThis <=> wholeSizeOther)
						   : (wholeSizeOther <=> wholeSizeThis);

	// Compare from most significant chunk downwards to account for potential precision mismatch
	size_t maxSize = std::max(chunks.size(), other.chunks.size());
	for (size_t i = 0; i < maxSize; i++) {
		// If length is different assume missing chunk = 0
		uint32_t chunkThis =
			(i < chunks.size()) ? getChunk(chunks.size() - i - 1) : 0;
		uint32_t chunkOther = (i < other.chunks.size())
								  ? other.getChunk(other.chunks.size() - i - 1)
								  : 0;

		if (chunkThis != chunkOther) {
			return (sign == 1) ? (chunkThis <=> chunkOther)
							   : (chunkOther <=> chunkThis);
		}
	}
	return std::strong_ordering::equal;
}

bool LongNumber::operator==(const LongNumber &other) const {
	return (*this <=> other) == std::strong_ordering::equal;
};

LongNumber LongNumber::operator+(const LongNumber &other) const {
	if (sign != other.sign) {
		if (sign == -1) return other - (-(*this)); // -a + b = b - a
		return *this - (-other);				   // a + -b = a - b
	}
	LongNumber result(0, std::max(fractionBits, other.fractionBits));
	result.sign = sign;
	size_t maxSize = std::max(chunks.size(), other.chunks.size());
	result.chunks.resize(maxSize);

	uint32_t carry = 0;
	for (int i = 0; i < maxSize; i++) {
		uint64_t sum = carry;
		if (i < chunks.size()) sum += chunks[i];
		if (i < other.chunks.size()) sum += other.chunks[i];

		carry = sum >> digitsPerChunk;
		result.chunks[i] = static_cast<uint32_t>(sum);
	}
	if (carry != 0) result.chunks.push_back(carry);
	return result;
}

LongNumber LongNumber::operator-() const {
	// -0 = +0
	if ((*this).chunks.empty()) return *this;
	LongNumber result = *this;
	result.sign = -result.sign;
	return result;
}

LongNumber LongNumber::operator-(const LongNumber &other) const {
	// -a - b -> -a + -b || a - (-b) -> a + b
	if (sign != other.sign) return *this + (-other);

	size_t maxPrecision = std::max(fractionBits, other.fractionBits);
	if (*this == other) {
		LongNumber result = LongNumber(0, maxPrecision);
		result.sign = 1;
		return result;
	}

	// If |a| < |b|, result is negative
	bool negateResult = (*this < other);
	const LongNumber &larger = negateResult ? other : *this;
	const LongNumber &smaller = negateResult ? *this : other;

	LongNumber result(0, maxPrecision);
	result.sign = negateResult ? -1 : 1;

	size_t maxSize = larger.chunks.size();
	result.chunks.resize(maxSize);

	int64_t borrow = 0;
	for (size_t i = 0; i < maxSize; i++) {
		int64_t diff = static_cast<int64_t>(larger.chunks[i]) - borrow;
		if (i < smaller.chunks.size()) {
			diff -= smaller.chunks[i];
		}

		if (diff < 0) {
			borrow = 1;
			diff += (1LL << digitsPerChunk);
		} else {
			borrow = 0;
		}

		result.chunks[i] = static_cast<uint32_t>(diff);
	}
	// Remove leading zero chunks
	result.truncateWholePart();
	return result;
}

LongNumber LongNumber::operator*(const LongNumber &other) const {
	// Round to digitsPerChunk
	// Prevent overflow of uint32_t by picking min
	uint32_t newPrecision = std::min(
		std::numeric_limits<u_int32_t>::max(),
		(getFractionChunks() + other.getFractionChunks()) * digitsPerChunk
	);
	uint32_t maxPrecisionBits = std::max(fractionBits, other.fractionBits);
	LongNumber result(0.0L, newPrecision);
	result.sign = sign * other.sign;

	// x * 0 = 0
	if (*this == 0 || other == 0) {
		result.setPrecision(maxPrecisionBits);
		return result;
	}
	result.chunks.resize(chunks.size() + other.chunks.size());

	for (int i = 0; i < chunks.size(); i++) {
		uint32_t carry = 0;
		for (int j = 0; j < other.chunks.size(); j++) {
			uint64_t mult = static_cast<uint64_t>(chunks[i]) * other.chunks[j] +
							carry + result.chunks[i + j];
			result.chunks[i + j] = static_cast<uint32_t>(mult);
			carry = mult >> digitsPerChunk;
		}
		result.chunks[i + other.chunks.size()] += carry;
	}
	result.truncateWholePart();
	// Set precision to max (not rounding by digitsPerChunk)
	result.setPrecision(maxPrecisionBits);
	return result;
}

LongNumber LongNumber::operator/(const LongNumber &other) const {
	if (other == 0) throw std::invalid_argument("Division by zero");

	uint32_t maxPrecision = std::max(fractionBits, other.fractionBits);

	// Work with absolute values
	LongNumber dividend = *this;
	dividend.sign = 1;
	dividend.setPrecision(maxPrecision);

	LongNumber divisor = other;
	divisor.sign = 1;
	divisor.setPrecision(maxPrecision);

	// Shift fraction part (full chunks)
	dividend <<= getFractionChunks() * digitsPerChunk;

	LongNumber quotient(0.0L, maxPrecision);
	quotient.sign = sign * other.sign;
	LongNumber remainder(0.0L, maxPrecision);

	// Bitwise division
	for (int i = dividend.chunks.size() * digitsPerChunk - 1; i >= 0; --i) {
		remainder <<= 1;
		remainder.chunks[0] |= (dividend.getBit(i) ? 1 : 0);

		if (remainder >= divisor) {
			remainder -= divisor;
			quotient.setBit(i);
		}
	}
	// Should not be neccessary, more of a precaution
	quotient.truncateWholePart();
	return quotient;
}

LongNumber &LongNumber::operator+=(const LongNumber &other) {
	*this = *this + other;
	return *this;
}

LongNumber &LongNumber::operator-=(const LongNumber &other) {
	*this = *this - other;
	return *this;
}

LongNumber &LongNumber::operator*=(const LongNumber &other) {
	*this = *this * other;
	return *this;
}

LongNumber &LongNumber::operator/=(const LongNumber &other) {
	*this = *this / other;
	return *this;
}

// *BIT SHIFTS*

LongNumber &LongNumber::operator<<=(int shift) {
	if (shift == 0) return *this;
	if (shift < 0) {
		*this >>= -shift;
		return *this;
	};

	uint32_t chunkShift = shift / digitsPerChunk;
	uint32_t bitShift = shift % digitsPerChunk;

	if (chunkShift > 0) chunks.insert(chunks.begin(), chunkShift, 0);
	if (bitShift == 0) return *this;

	uint32_t carry = 0;
	for (size_t i = 0; i < chunks.size(); i++) {
		uint32_t newCarry = chunks[i] >> (digitsPerChunk - bitShift);
		chunks[i] = (chunks[i] << bitShift) | carry;
		carry = newCarry;
	}

	if (carry) chunks.push_back(carry);
	return *this;
}
LongNumber &LongNumber::operator>>=(int shift) {
	if (shift == 0) return *this;
	if (shift < 0) {
		*this <<= -shift;
		return *this;
	};

	uint32_t chunkShift = shift / digitsPerChunk;
	uint32_t bitShift = shift % digitsPerChunk;

	if (chunkShift >= chunks.size()) {
		chunks.clear();
		allocateFraction();
		return *this;
	}
	// Remove the first `chunkShift` chunks
	chunks.erase(chunks.begin(), chunks.begin() + chunkShift);
	// Make sure that there are at least `getFractionChunks()` chunks
	allocateFraction();
	if (bitShift == 0) return *this;

	uint32_t carry = 0;
	for (int i = chunks.size() - 1; i >= 0; i--) {
		// 0 <= bitShift < digitsPerChunk No UB
		uint32_t newCarry = chunks[i] << (digitsPerChunk - bitShift);
		chunks[i] = (chunks[i] >> bitShift) | carry;
		carry = newCarry;
	}
	truncateWholePart();
	return *this;
}
LongNumber operator<<(LongNumber lhs, int shift) {
	lhs <<= shift;
	return lhs;
}
LongNumber operator>>(LongNumber lhs, int shift) {
	lhs >>= shift;
	return lhs;
}

// *COPY OPERATOR*

LongNumber &LongNumber::operator=(const LongNumber other) {
	sign = other.sign;
	fractionBits = other.fractionBits;
	chunks = other.chunks;
	return *this;
}
} // namespace LongArithm
