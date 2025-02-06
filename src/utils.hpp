#pragma once

#include <functional>

template<typename T>
std::function<bool()> isEquals(T x, T y) {
    return [x, y]() { return x == y; };
}
