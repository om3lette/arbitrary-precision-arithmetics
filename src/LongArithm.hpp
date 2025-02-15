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

	void setBit(int index);
	bool getBit(int index) const;

	void allocateFraction(void);
	void truncateWholePart(void);

	inline char digitToChar(const int d) const;
	inline u_int32_t getFractionChunks(void) const;

	// Used as a helper for fromBinaryString
	void convertBinaryString(const std::string &input);
	void fromBinaryString(const std::string &input, uint32_t fractionalBits);

  public:
	LongNumber();
	LongNumber(long double input, uint32_t fractionBits = 96);
	LongNumber(const std::string input, uint32_t _fractionBits = 96);

	LongNumber(const LongNumber &other) = default;
	LongNumber &operator=(const LongNumber other);
	~LongNumber() = default;

	void setPrecision(uint32_t precision);
	LongNumber withPrecision(uint32_t precision);
	uint32_t getChunk(uint32_t index) const;

	LongNumber pow(uint32_t power) const;
	LongNumber sqrt(void) const;

	void printChunks(void) const;
	const std::string toString(void) const;

	std::strong_ordering operator<=>(const LongNumber &other) const;
	bool operator==(const LongNumber &other) const;

	LongNumber &operator<<=(int shift);
	LongNumber &operator>>=(int shift);

	LongNumber operator+(const LongNumber &other) const;
	LongNumber operator-(const LongNumber &other) const;
	LongNumber operator*(const LongNumber &other) const;
	LongNumber operator/(const LongNumber &other) const;

	LongNumber &operator+=(const LongNumber &other);
	LongNumber &operator-=(const LongNumber &other);
	LongNumber &operator*=(const LongNumber &other);
	LongNumber &operator/=(const LongNumber &other);

	LongNumber operator-() const;
};
LongNumber operator""_longnum(long double value);
LongNumber operator<<(LongNumber lhs, int shift);
LongNumber operator>>(LongNumber lhs, int shift);
// std::ostream &operator<<(std::ostream &os, const LongNumber &number);
} // namespace LongArithm
