#pragma once

#include <functional>
#include <iostream>
#include <sstream>

template <typename T> std::function<bool()> isEquals(T x, T y) {
	return [x, y]() { return x == y; };
}

std::function<bool()>
compareOutput(std::function<void()> func, const std::string &expectedValue) {
	return [func, expectedValue]() {
		std::stringstream buffer;
		std::streambuf *old = std::cout.rdbuf();
		std::cout.rdbuf(buffer.rdbuf());

		func();

		std::cout.rdbuf(old);
		return buffer.str() == expectedValue;
	};
}
