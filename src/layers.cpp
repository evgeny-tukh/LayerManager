#include <cstdint>

#include <Windows.h>

#include <opencv.hpp>

#include "layers.hpp"

namespace {
std::string emptyStr;
}
bool loadTiff(const char *file) {
    std::string filePath(file);
    cv::Mat img = cv::imread(filePath, cv::IMREAD_UNCHANGED);

    if (img.empty())
        return false;

    uint8_t *pixelData = img.data;
    size_t totalBytes = img.total() * img.elemSize();

    return true;
}

void Layers::addLayer(const char *file) {
    std::string filePath(file);
    cv::Mat img = cv::imread(filePath, cv::IMREAD_UNCHANGED);

    if (!img.empty())
        _container.emplace_back(ImgItem(filePath, img));
}

const std::string& Layers::name(size_t index) const {
    if (index >= _container.size())
        return emptyStr;
        
    return _container[index].first;
}

const Layers::OptImage Layers::img(size_t index) const {
    if (index >= _container.size())
        return {};

    return _container[index].second;
}

const Layers::OptPixel Layers::pixel(size_t index, int x, int y) const {
    auto image = img(index);

    if (!image.has_value())
        return {};

    return image.value().at<cv::Vec3b>(y, x);
}

const size_t Layers::width(size_t index) const {
    auto image = img(index);

    if (!image.has_value())
        return 0;
        
    return image.value().cols;
}

const size_t Layers::height(size_t index) const {
    auto image = img(index);

    if (!image.has_value())
        return 0;
        
    return image.value().rows;
}

void Layers::drawAt(HDC ctx, size_t index) {
    auto image = img(index);

    if (!image.has_value())
        return;

    cv::Mat& img = image.value();

    int width = img.cols;
    int height = img.rows;

    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            cv::Vec3b& pixel = img.at<cv::Vec3b>(y, x);

            auto blue = pixel[0];
            auto green = pixel[1];
            auto red = pixel[2];

            SetPixel(ctx, x, y, RGB(red, green, blue));
        }
    }
}

void Layers::drawAtFast(HWND wnd, size_t index) {
    HDC ctx = GetDC(wnd);
    auto image = img(index);
    RECT client;

    GetClientRect(wnd, &client);

    if (!image.has_value())
        return;

    cv::Mat& img = image.value();

    int width = img.cols;
    int height = img.rows;

    std::vector<uint32_t> raster(width * height);

    for (int y = 0, index = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x, ++index) {
            cv::Vec3b& pixel = img.at<cv::Vec3b>(y, x);

            auto blue = pixel[0];
            auto green = pixel[1];
            auto red = pixel[2];

            raster[index] = RGB(red, green, blue);
        }
    }

    BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = static_cast<DWORD>(img.total() * img.elemSize());

    SetDIBitsToDevice(ctx, 0, 0, width, height, 0, 0, 0, height, raster.data(), &bmi, DIB_RGB_COLORS);
    ReleaseDC(wnd, ctx);
}

void Layers::drawAt(HWND wnd, size_t index) {
    auto ctx = GetDC(wnd);
    
    drawAt(ctx, index);

    ReleaseDC(wnd, ctx);
}