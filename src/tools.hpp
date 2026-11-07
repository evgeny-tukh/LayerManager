#pragma once

#include <cstdint>

#include "defs.hpp"

Color rgb2hsv(Color rgb);
Color rgb2hsv(uint8_t red, uint8_t green, uint8_t blue);
Image correctColors(Image& image, Color tranparentColor, Color nonTransparentColor);

inline Color abstractColor2color(const AbstractColor& clr) { return Color(clr.blue, clr.green, clr.red); }
AbstractColor color2abstractcolor(const Color& clr);

void populatePixelBuffer(const Image& img, PixelBuf& pixelBuffer, int channels = 3);

int convertToRgbaWithTransparentColor(Image& img, const Color& transparentColor);

void rotate(const Image& src, Image& dest, int angle);

const cv::Vec3b& pixel(const cv::Mat& img, int x, int y);

uint8_t *pixelDirect(const cv::Mat& img, int x, int y);

const AbstractColor& getRgbQuad(const PixelBuf& pixelBuffer, int width, int x, int y);
