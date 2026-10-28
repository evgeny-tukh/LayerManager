#include <cstdint>

#include <Windows.h>

#include <opencv.hpp>

#include "layers.hpp"

namespace {
std::string emptyStr;
const RGBQUAD transpColor { 255, 255, 255, 0 };
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
    _container.emplace_back(file);
}

const std::string& Layers::name(size_t index) const {
    if (index >= _container.size())
        return emptyStr;
        
    return _container[index].name();
}

const cv::Mat& Layers::image(size_t index) const {
    if (index >= _container.size())
        return {};

    return _container[index].image();
}

const cv::Vec3b& Layers::pixel(size_t index, int x, int y) const {
    if (index >= _container.size())
        return {};

    return _container[index].image();
}

const size_t Layers::width(size_t index) const {
    if (index >= _container.size())
        return 0;

    return _container[index].width();
}

const size_t Layers::height(size_t index) const {
    if (index >= _container.size())
        return 0;

    return _container[index].height();
}

void Layers::drawAt(HDC ctx, size_t index) {
    if (index < _container.size())
        _container[index].draw(ctx);
}

void Layers::drawAtFast(HWND wnd, size_t index) {
    #if 0
    HDC ctx = GetDC(wnd);
    auto image = image(index);
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
    #endif
}

void Layers::drawAt(HWND wnd, size_t index) {
    auto ctx = GetDC(wnd);
    
    drawAt(ctx, index);

    ReleaseDC(wnd, ctx);
}

void Layers::compose() {
    int maxWidth = 0;
    int maxHeight = 0;

    for (const auto& layer: _container) {
        if (layer.width() > maxWidth)
            maxWidth = static_cast<int>(layer.width());

        if (layer.height() > maxHeight)
            maxHeight = static_cast<int>(layer.height());
    }

    _composition.setSize(maxWidth, maxHeight, transpColor);

    for (int x = 0; x < maxWidth; ++x) {
        for (int y = 0; y < maxWidth; ++y) {
            for (const auto& layer: _container) {
                const auto& pixel = layer.pixel(x, y);

                if (memcmp(&pixel, &transpColor, 3) != 0) {
                    RGBQUAD rgb;

                    rgb.rgbBlue = pixel[0];
                    rgb.rgbGreen = pixel[1];
                    rgb.rgbRed = pixel[0];
                    rgb.rgbReserved = 0;

                    _composition.setPixel(x, y, rgb);
                }
            }
        }
    }
}

void Layers::erase(size_t index) {
    if (index < _container.size())
        _container.erase(_container.begin() + index);
}

void Layers::swapItems(size_t index, int direction) {
    int index2 = (signed) index + direction;

    if (index2 >= 0 && (signed) index2 < _container.size())
        std::swap (_container[index], _container[index2]);
}