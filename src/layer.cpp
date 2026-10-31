#include <sstream>

#include <opencv2/geometry/2d.hpp>

#include "layer.hpp"

namespace {
const cv::Vec3b emptyPixel { 0, 0, 0 };
const RGBQUAD defFill { 127, 127, 127, 0 };

const cv::Vec3b& pixel(const cv::Mat& img, int x, int y) {
    if (img.empty())
        return emptyPixel;

    return img.at<cv::Vec3b>(y, x);
}

void populatePixelBuffer(const cv::Mat& img, Layer::PixelBuf& pixelBuffer) {
    pixelBuffer.resize(img.cols * img.rows);

    for (int y = 0; y < img.rows; ++y) {
        for (int x = 0; x < img.cols; ++x) {
            int index = static_cast<int>(y * img.rows + x);

            auto& destPixel = pixelBuffer[index];
            const auto& srcPixel = pixel(img, x, y) ;

            destPixel.rgbRed = srcPixel[2];
            destPixel.rgbGreen = srcPixel[1];
            destPixel.rgbBlue = srcPixel[0];
            destPixel.rgbReserved = 0;
        }
    }
}

void rotate(const cv::Mat& src, cv::Mat& dest, int angle) {
    double turn = -angle;
    double centerX = src.cols * 0.5;
    double centerY = src.rows * 0.5;
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

}

Layer::Layer(const char *imgFile): _imgFile(imgFile), _xOffset(0), _yOffset(0), _turnAngle(0), _fillColor(defFill), _showRotated(false) {
    _img = cv::imread(imgFile, cv::IMREAD_UNCHANGED);

    if (_img.empty()) 
        return;
        
    populatePixelBuffer(_img, _pixelBuffer);

    if (_turnAngle != 0)
        applyTurnAngle();
}

Layer::~Layer() {
}

void Layer::applyTurnAngle() {
    rotate(_img, _rotatedImg, _turnAngle);
    populatePixelBuffer(_rotatedImg, _rotatedPixelBuffer);
}

void Layer::setTurnAngle(int angle) {
    _turnAngle = angle;

    applyTurnAngle();
}

void Layer::draw(HDC hdc) {
    const auto& img = _showRotated ? _rotatedImg : _img;
    const auto& pixelBuf = _showRotated ? _rotatedPixelBuffer : _pixelBuffer;

    if (img.cols == 0 && img.rows == 0)
        return;

    HDC screenDC = GetDC(HWND_DESKTOP);
    HDC tempDC = CreateCompatibleDC(screenDC);
    HBITMAP tempBmp = CreateCompatibleBitmap(screenDC, img.cols, img.rows);

    ReleaseDC(HWND_DESKTOP, screenDC);

    BITMAPINFO bmi;

    memset(&bmi, 0, sizeof(bmi));

    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = img.cols;
    bmi.bmiHeader.biHeight = -img.rows;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    SelectObject(tempDC, tempBmp);
    SetDIBits(tempDC, tempBmp, 0, img.rows, pixelBuf.data(), &bmi, DIB_RGB_COLORS);
    BitBlt(hdc, _xOffset, _yOffset, img.cols, img.rows, tempDC, 0, 0, SRCCOPY);
    DeleteObject(tempBmp);
    DeleteDC(tempDC);
}

const cv::Vec3b& Layer::pixel(int x, int y, bool rotated) const {
    const auto& img = rotated ? _img : _rotatedImg;
    return ::pixel(img, x, y);
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

void Layer::getOffset(int& xOffset, int& yOffset) const {
    xOffset = _xOffset;
    yOffset = _yOffset;
}

void Layer::setOffset(int xOffset, int yOffset) {
    _xOffset = xOffset;
    _yOffset = yOffset;
}
