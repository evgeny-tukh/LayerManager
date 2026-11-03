#include <sstream>

#include <opencv2/geometry/2d.hpp>

#include "layer.hpp"

namespace {
const cv::Vec3b emptyPixel { 0, 0, 0 };
const RGBQUAD defFill { 127, 127, 127, 0 };

int convertToRgba(cv::Mat& img) {
    cv::Mat continuousImg;
    if (!img.isContinuous()) {
        continuousImg = img.clone();
    } else {
        continuousImg = img;
    }

    cv::flip(continuousImg, img, 0);
    
    return img.channels();
}

int convertToRgbaWithTransparentColor(cv::Mat& img, cv::Scalar transparentColor) {
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

const cv::Vec3b& pixel(const cv::Mat& img, int x, int y) {
    if (img.empty())
        return emptyPixel;

    return img.at<cv::Vec3b>(y, x);
}

uint8_t *pixelDirect(const cv::Mat& img, int x, int y) {
    return img.data + (y * img.step.p[0] + x * 3);
}

void populatePixelBuffer(const cv::Mat& img, Layer::PixelBuf& pixelBuffer, int channels = 3) {
    pixelBuffer.resize(img.cols * img.rows);

    memcpy(pixelBuffer.data(), img.data, pixelBuffer.size() * sizeof(RGBQUAD));
    return;

    for (int y = 0; y < img.rows - 5; ++y) {
        const uchar *line = img.data + (y * img.step.p[0]);

        for (int x = 0; x < img.cols - 5; ++x) {
            int index = static_cast<int>(y * img.cols + x);

            auto& destPixel = pixelBuffer[index];

            destPixel.rgbRed = line[x*3];
            destPixel.rgbGreen = line[x*3+1];
            destPixel.rgbBlue = line[x*3+2];
            destPixel.rgbReserved = 0;
        }
    }
}

RGBQUAD getRgbQuad(const Layer::PixelBuf& pixelBuffer, int width, int x, int y) {
    int index = static_cast<int>(y * width + x);

    return pixelBuffer[index];
}

void rotate(const cv::Mat& src, cv::Mat& dest, int angle) {
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

}

void Layer::initImage(const char *imgFile) {
    _img = cv::imread(imgFile, cv::IMREAD_COLOR);

    if (_img.empty()) 
        return;
        
    cv::Scalar transpColor { 255, 255, 255 };
    int channels = convertToRgbaWithTransparentColor(_img, transpColor);
    populatePixelBuffer(_img, _pixelBuffer, channels);

    if (_turnAngle != 0)
        applyTurnAngle();
}

Layer::Layer(const char *imgFile): Layer(imgFile, 0, 0, defFill, 0) {
}

Layer::Layer(
    const char *imgFile,
    int xOffset,
    int yOffset,
    RGBQUAD fillColor,
    int turnAngle
): _imgFile(imgFile), _xOffset(xOffset), _yOffset(yOffset), _fillColor(fillColor), _turnAngle(turnAngle), _showRotated(false) {
    initImage(imgFile);
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

void Layer::draw(HDC hdc, const PixelBuf& pixelBuf, int x, int y, int width, int height) {
    HDC screenDC = GetDC(HWND_DESKTOP);
    HDC tempDC = CreateCompatibleDC(screenDC);
    HBITMAP tempBmp = CreateCompatibleBitmap(screenDC, width, height);

    ReleaseDC(HWND_DESKTOP, screenDC);

    BITMAPINFO bmi;

    memset(&bmi, 0, sizeof(bmi));

    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    SelectObject(tempDC, tempBmp);
    SetDIBits(tempDC, tempBmp, 0, height, pixelBuf.data(), &bmi, DIB_RGB_COLORS);
    BitBlt(hdc, x, y, width, height, tempDC, 0, 0, SRCCOPY);
    DeleteObject(tempBmp);
    DeleteDC(tempDC);
}

void Layer::draw(HDC hdc, int x, int y) {
    const auto& img = _showRotated ? _rotatedImg : _img;
    const auto& pixelBuf = _showRotated ? _rotatedPixelBuffer : _pixelBuffer;

    if (img.cols == 0 && img.rows == 0)
        return;

    draw(hdc, pixelBuf, _xOffset - x, _yOffset - y, img.cols, img.rows);
}

const cv::Vec3b& Layer::pixel(int x, int y, bool rotated) const {
    const auto& img = rotated ? _rotatedImg : _img;
    return ::pixel(img, x, y);
}

const uint8_t *Layer::pixelDirect(int x, int y, bool rotated) const {
    const auto& img = rotated ? _rotatedImg : _img;
    return ::pixelDirect(img, x, y);
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

RGBQUAD Layer::getRgbQuad(int x, int y, bool rotated) {
    const auto& img = rotated ? _rotatedImg : _img;
    const auto& buf = rotated ? _rotatedPixelBuffer: _pixelBuffer;

    return ::getRgbQuad(buf, img.cols, x, y);
}
