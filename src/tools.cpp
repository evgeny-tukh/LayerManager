#include <cstdint>

#include <opencv.hpp>
#include <imgproc.hpp>

#include "defs.hpp"

Color rgb2hsv(Color rgb) {
    Image bgrColor(1, 1, CV_8UC3, rgb);
    Image hsvColor;

    cvtColor(bgrColor, hsvColor, cv::COLOR_BGR2HSV);

    return Color(hsvColor.data[0], hsvColor.data[1], hsvColor.data[2]);
}

Color rgb2hsv(uint8_t red, uint8_t green, uint8_t blue) {
    return rgb2hsv(Color(blue, green, red));
}

Image correctColors(Image& image, Color tranparentColor, Color nonTransparentColor) {
    Image rangesFromTransparent;
    cv::absdiff(image, tranparentColor, rangesFromTransparent); 

    Image grayscaledRangesFromTransparent;
    cv::cvtColor(rangesFromTransparent, grayscaledRangesFromTransparent, cv::COLOR_BGR2GRAY);

    Image bw;
    cv::threshold(grayscaledRangesFromTransparent, bw, 15, 255, cv::THRESH_BINARY_INV);

    return bw;
}

AbstractColor color2abstractcolor(const Color& clr) {
    AbstractColor result;

    result.alpha = 0;
    result.blue = clr[0];
    result.green = clr[1];
    result.red = clr[2];

    return result;
}
