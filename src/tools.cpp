#include <cstdint>
#include <string>
#include <sstream>

#include <opencv.hpp>
#include <imgproc.hpp>
#include <opencv2/geometry/2d.hpp>

#include "defs.hpp"

namespace {

const cv::Vec3b emptyPixel { 0, 0, 0 };
const AbstractColor defFill { 127, 127, 127, 0 };

}

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
    result.blue = static_cast<uint8_t>(clr[0]);
    result.green = static_cast<uint8_t>(clr[1]);
    result.red = static_cast<uint8_t>(clr[2]);

    return result;
}

void populatePixelBuffer(const Image& img, PixelBuf& pixelBuffer, int channels) {
    pixelBuffer.resize(img.cols * img.rows);

    memcpy(pixelBuffer.data(), img.data, pixelBuffer.size() * sizeof(AbstractColor));
    return;

    for (int y = 0; y < img.rows - 5; ++y) {
        const uchar *line = img.data + (y * img.step.p[0]);

        for (int x = 0; x < img.cols - 5; ++x) {
            int index = static_cast<int>(y * img.cols + x);

            auto& destPixel = pixelBuffer[index];

            destPixel.red = line[x*3];
            destPixel.green = line[x*3+1];
            destPixel.blue = line[x*3+2];
            destPixel.alpha = 0;
        }
    }
}

int convertToRgbaWithTransparentColor(Image& img, const Color& transparentColor) {
    int ch = 0;
    if (img.channels() != 3) {
        img = img.clone();
        ch = img.channels();
    }

    // Create the mask based on the transparent color
    cv::Mat mask;
    cv::inRange(img, transparentColor, transparentColor, mask);

    // Invert the mask
    cv::Mat alpha;
    cv::bitwise_not(mask, alpha);

    // Split color channels
    std::vector<cv::Mat> bgrChannels;
    cv::split(img, bgrChannels);

    // Append alpha channel
    bgrChannels.push_back(alpha);

    // Merge channels back
    cv::merge(bgrChannels, img);
    ch = img.channels();

    return img.channels();
}

void rotate(const Image& src, Image& dest, int angle) {
    float turn = static_cast<float>(-angle);
    float centerX = src.cols * 0.5f;
    float centerY = src.rows * 0.5f;
    cv::Point2f center(centerX, centerY);
    cv::Mat rotationMatrix = cv::getRotationMatrix2D(center, turn, 1.0);
    std::stringstream ss;
    ss << rotationMatrix << '\n';
    auto a = ss.str();
    dest = cv::Mat::zeros(src.rows, src.cols, src.type());
    cv::Mat(srcAligned);
    src.copyTo(srcAligned);
    cv::warpAffine(src, dest, rotationMatrix, src.size());
    cv::imwrite("debug_result.png", dest);
}

const cv::Vec3b& pixel(const cv::Mat& img, int x, int y) {
    if (img.empty())
        return emptyPixel;

    return img.at<cv::Vec3b>(y, x);
}

uint8_t *pixelDirect(const cv::Mat& img, int x, int y) {
    return img.data + (y * img.step.p[0] + x * 3);
}

const AbstractColor& getRgbQuad(const PixelBuf& pixelBuffer, int width, int x, int y) {
    int index = static_cast<int>(y * width + x);

    return pixelBuffer[index];
}

std::string createDefLayerName() {
    static uint32_t layerCount = 1;
    std::stringstream ss;

    ss << "New Layer " << (layerCount++);

    return ss.str();
}
