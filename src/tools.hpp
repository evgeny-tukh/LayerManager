#pragma once

#include <cstdint>

#include "defs.hpp"

Color rgb2hsv(Color rgb);
Color rgb2hsv(uint8_t red, uint8_t green, uint8_t blue);
Image correctColors(Image& image, Color tranparentColor, Color nonTransparentColor);
