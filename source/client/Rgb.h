#pragma once

#include <stdint.h>

class Rgb {
public:
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

namespace color {
static constexpr Rgb black = {0, 0, 0};
static constexpr Rgb red = {255, 0, 0};
static constexpr Rgb green = {0, 255, 0};
static constexpr Rgb blue = {0, 0, 255};
static constexpr Rgb white = {255, 255, 255};
} // namespace color
