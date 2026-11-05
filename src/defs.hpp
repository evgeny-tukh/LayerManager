#pragma once

#include <cstdint>
#include <vector>

#include <opencv.hpp>

using Color = cv::Scalar;
using Image = cv::Mat;
using Size = cv::Size;

#pragma pack(1)
struct AbstractColor {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t alpha;

    bool operator==(const AbstractColor& right) const {
        return blue == right.blue && red == right.red && green == right.green;
    }

    bool operator!=(const AbstractColor& right) const {
        return !(*this == right);
    }
};
#pragma pack()

using PixelBuf = std::vector<AbstractColor>;

struct LayerBaseProps {
    int x = 0;
    int y = 0;
    int width = 100;
    int height = 100;
    int turnAngle = 0;
    AbstractColor transpColor = { 255, 255, 255, 0 };
    AbstractColor fillColor = { 128, 128, 128, 0 };
};

inline Color defTransparentColor() { return { 255, 255, 255 }; }
inline Color defFillColor() { return { 128, 128, 128 }; }
inline Size getDefSize() { return { 0, 0 }; }
inline Size getZeroShift() { return { 0, 0 }; }

