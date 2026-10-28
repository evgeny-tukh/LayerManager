#include "layer.hpp"

namespace {
const cv::Vec3b emptyPixel { 0, 0, 0 };
}

Layer::Layer(const char *imgFile): _imgFile(imgFile), _xOffset(0), _yOffset(0) {
    _img = cv::imread(imgFile, cv::IMREAD_UNCHANGED);

    if (!_img.empty()) {
        _pixelBuffer.resize(width() * height());

        for (int y = 0; y < height(); ++y) {
            for (int x = 0; x < width(); ++x) {
                int index = static_cast<int>(y * width() + x);

                auto& destPixel = _pixelBuffer[index];
                const auto& srcPixel = pixel(x, y) ;

                destPixel.rgbRed = srcPixel[2];
                destPixel.rgbGreen = srcPixel[1];
                destPixel.rgbBlue = srcPixel[0];
                destPixel.rgbReserved = 0;
            }
        }
    }
}

Layer::~Layer() {
}

void Layer::draw(HDC hdc) {
    if (_img.cols == 0 && _img.rows == 0)
        return;

    HDC screenDC = GetDC(HWND_DESKTOP);
    HDC tempDC = CreateCompatibleDC(screenDC);
    HBITMAP tempBmp = CreateCompatibleBitmap(screenDC, _img.cols, _img.rows);

    ReleaseDC(HWND_DESKTOP, screenDC);

    BITMAPINFO bmi;

    memset(&bmi, 0, sizeof(bmi));

    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = _img.cols;
    bmi.bmiHeader.biHeight = _img.rows;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    SelectObject(tempDC, tempBmp);
    SetDIBits(tempDC, tempBmp, 0, _img.rows, _pixelBuffer.data(), &bmi, DIB_RGB_COLORS);
    BitBlt(hdc, 0, 0, _img.cols, _img.rows, tempDC, _xOffset, _yOffset, SRCCOPY);
    DeleteObject(tempBmp);
    DeleteDC(tempDC);
}

const cv::Vec3b& Layer::pixel(int x, int y) const {
    if (_img.empty())
        return emptyPixel;

    return _img.at<cv::Vec3b>(y, x);
}

void Layer::setPixel(int x, int y, RGBQUAD clr) {
    int index = static_cast<int>(y * width() + x);
    _pixelBuffer[index] = clr;
}

void Layer::setSize(int width, int height, RGBQUAD transpClr) {
    _img.cols = width;
    _img.rows = height;

    _pixelBuffer.resize(width * height);

    for (size_t i = 0; i < _pixelBuffer.size(); _pixelBuffer[i++] = transpClr);
}