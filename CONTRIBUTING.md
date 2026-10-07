# Contributing

## Build
cmake -B build -DCMAKE_CXX_STANDARD=23
cmake --build build

## Test
ctest --test-dir build --output-on-failure

## Code Style
- C++23, clang-format (файл .clang-format)
- -Wall -Wextra -Werror обязательны