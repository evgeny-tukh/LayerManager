#pragma once

#include <string>

#include "defs.hpp"

struct Stage {
    Stage() = default;
    Stage(const Stage& source);

    Color transparent { 255, 255, 255 };
    Color fill { 128, 128, 128 };
    Size size { 0, 0 };
    Size shift { 0, 0 };
    Image image;
    PixelBuf pixelBuf;
};
