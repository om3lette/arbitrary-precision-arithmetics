# Arbitrary precision arithmetics library

Library written as a homework assignment during HSE: SE C++ course.

> [!WARNING]
> This is my first experience with C++

## Functionality

Library provides a class `LongNumber` that supports the following operations: (`+`, `-`, `*`, `/`, `<<`, `>>`).

## Precision

Precision is fixed and is provided on initialization

```
LongNum x(30.0L, 0);  // Int like structure
LongNum x(30.0L);  // Defaults to 96 bits precision
```

When doing multiplication (`*`) or division (`/`) the result has the maximum precision of the two numbers.\
It can be changed at a later date using\

- `setPrecision` (changed `fractionBits` inplace and resizes vector accordingly)
- `withPrecision` (returns a copy with the aforementioned properties)

## Initialization

There are multiple ways to create `LongNumber`

```
LongNum x = 2.0L_longnum; // Precision defaults to 96 bits
LongNum y = LongNum(2.0L, 32); // From long double
LongNum z = LongNum(2, 32);
LongNum x = LongNum("10", 32); // Or binary string
```

## Output

One can use `toBinaryString` or `toString` method to get a binary and decimal representation respectively.\
`printChunks` is also available and can be used to visualize the insides of a number with its current `fractionBits` aka precision and fraction chunks

## Makefile

### Targets

- `test` - runs test executeable
- `test.build` - builds test executable
- `coverage` - `test.build` is a prerequisite. Runs tests and generates coverage report
- `pi` - runs pi executable if present
- `pi.build` - builds pi executable
- `pi.profile` - runs profiling to later analyse using kcachegrind
- `clean` - deletes build/coverage folders

### Arguments:

- `BUILD` values: `release`, `debug` - adds optimization flags when compiling
- `DIGITS` values: any `integer > 0`. Used in `pi` target to set precision
