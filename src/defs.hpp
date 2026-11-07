#pragma once

#include <cstdint>
#include <vector>
#include <string>

#include <opencv.hpp>

using Color = cv::Scalar;
using Image = cv::Mat;
using Size = cv::Size;

std::string createDefLayerName();

#pragma pack(1)
struct AbstractColor {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t alpha;

    AbstractColor() = default;
    AbstractColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0): red(r), green(g), blue(b), alpha(a) {}

    bool operator==(const AbstractColor& right) const {
        return blue == right.blue && red == right.red && green == right.green;
    }

    bool operator!=(const AbstractColor& right) const {
        return !(*this == right);
    }

    std::string toString() {
        std::stringstream ss;

        ss << "(R:" << red << ";G:" << green << ";B:" << blue << ')';

        return ss.str();
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
    std::string name = createDefLayerName();
};

inline Color defTransparentColor() { return { 255, 255, 255 }; }
inline AbstractColor defTransparentColorA() { return { 255, 255, 255, 0 }; }
inline Color defFillColor() { return { 128, 128, 128 }; }
inline AbstractColor defFillColorA() { return { 128, 128, 128, 0 }; }
inline Size getDefSize() { return { 0, 0 }; }
inline Size getZeroShift() { return { 0, 0 }; }


