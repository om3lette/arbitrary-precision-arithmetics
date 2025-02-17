#include "pi.hpp"
#include <cmath>
#include <iostream>
#include <stdexcept>

using namespace LongArithm;

namespace pi {
uint32_t decimalToBinaryPrecision(uint32_t decimalDigits) {
	// Slightly above log2(10) as it was not enough
	return std::ceil(decimalDigits * 3.35);
}

// Calculate pi using Chudnovsky's series
// This calculates it in fixed point, using the value for one passed in
// Credits: https://www.craig-wood.com/nick/articles/pi-chudnovsky/
LongNumber calculatePi(const uint32_t precision) {
	LongNumber two(2, 0);
	LongNumber six(6, 0);

	LongNumber k(1);
	LongNumber a_k = LongNumber(1, precision);
	LongNumber a_sum = LongNumber(1, precision);
	LongNumber b_sum = 0;
	const LongNumber C = 640320;
	const LongNumber C3_OVER_24 = C.pow(3) / 24;
	while (a_k != 0) {
		LongNumber sixK = k * six;
		a_k *= -(sixK - 5) * (two * k - 1) * (sixK - 1);
		a_k /= k.pow(3) * C3_OVER_24;
		a_sum += a_k;
		b_sum += k * a_k;
		k += 1;
	}
	LongNumber total = a_sum * 13591409 + b_sum * 545140134;
	LongNumber pi =
		(426880.0_longnum * (10005.0_longnum).withPrecision(precision).sqrt()) /
		total;
	return pi;
}
} // namespace pi
