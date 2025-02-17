#include "pi.hpp"
#include <cmath>

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
	LongArithm::LongNumber pi =
		pi::calculatePi(pi::decimalToBinaryPrecision(precision));
	std::cout << pi.toString(precision) << '\n';
	return 0;
}