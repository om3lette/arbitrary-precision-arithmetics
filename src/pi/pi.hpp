#ifndef LONGNUM_HPP
#define LONGNUM_HPP

#include "../LongArithm.hpp"
#include <cinttypes>

namespace pi {
uint32_t decimalToBinaryPrecision(uint32_t decimalDigits);
LongArithm::LongNumber calculatePi(const uint32_t precision);
} // namespace pi

#endif