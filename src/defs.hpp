#pragma once

#include <opencv.hpp>

using Color = cv::Scalar;
using Image = cv::Mat;
using Size = cv::Size;

inline Color defTransparentColor() { return { 255, 255, 255 }; }
inline Color defFillColor() { return { 128, 128, 128 }; }
inline Size getDefSize() { return { 0, 0 }; }
inline Size getZeroShift() { return { 0, 0 }; }

