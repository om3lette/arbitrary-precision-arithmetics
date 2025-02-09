#pragma once

#include <compare>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#define digitsPerChunk 32

namespace LongArithm {
class LongNumber {
  private:
	std::vector<uint32_t> chunks;
	short sign;
	uint32_t fractionBits;

	void allocateFraction(void);
	void truncateWholePart(void);

	inline char digitToChar(const int d) const;
	inline int getFractionChunks(void) const;

	void parseString(const std::string &input);
	void fromString(const std::string &input, uint32_t fractionalBits);

  public:
	LongNumber();
	LongNumber(long double input, int _fractionBits = 96);
	LongNumber(const std::string input, int _fractionBits);

	~LongNumber() = default;

	void printChunks(void) const;
	const std::string toString(void) const;
	void setPrecision(uint32_t precision);

	std::strong_ordering operator<=>(const LongNumber &other) const;
	bool operator==(const LongNumber &other) const;

	LongNumber &operator<<=(int shift);
	LongNumber &operator>>=(int shift);

	LongNumber operator+(const LongNumber &other) const;
	LongNumber operator-(const LongNumber &other) const;
	LongNumber operator*(const LongNumber &other) const;

	LongNumber &operator+=(const LongNumber &other);
	LongNumber &operator-=(const LongNumber &other);
	LongNumber &operator*=(const LongNumber &other);

	LongNumber operator-() const;
};
LongNumber operator""_longnum(long double value);
LongNumber operator<<(LongNumber lhs, int shift);
LongNumber operator>>(LongNumber lhs, int shift);
// std::ostream &operator<<(std::ostream &os, const LongNumber &number);
} // namespace LongArithm
