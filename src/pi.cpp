#include "LongArithm.hpp"
#include <iostream>
#include <stdexcept>

using namespace LongArithm;

// Calculate pi using Chudnovsky's series
// This calculates it in fixed point, using the value for one passed in
// Credits: https://www.craig-wood.com/nick/articles/pi-chudnovsky/
LongNumber calculatePi(const unsigned precision) {
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

int main(int argc, char **argv) {
	if (argc < 2) {
		std::cerr << "Precision must be specified for the program to run\n";
		return 1;
	}
	const std::string arg = argv[1];
	int precision = 0;
	try {
		precision = std::stoi(arg);
	} catch (const std::invalid_argument &ex) {
		std::cerr << "Invalid precision: " << arg << '\n';
	} catch (const std::out_of_range &ex) {
		std::cerr << "Precision out of range: " << arg << '\n';
	}

	if (precision <= 0) {
		std::cerr << "Precision must be > 0\n";
		return 1;
	}
	LongNumber pi = calculatePi(static_cast<unsigned>(precision));
	std::cout << pi.toString() << '\n';
	return 0;
}