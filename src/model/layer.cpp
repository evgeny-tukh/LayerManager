#include <sstream>

#include <opencv2/geometry/2d.hpp>

#include "layer.hpp"
#include "win_tools.hpp"
#include "tools.hpp"
#include "from_file.hpp"
#include "rectangle.hpp"

namespace {
const cv::Vec3b emptyPixel { 0, 0, 0 };
const AbstractColor defFill { 127, 127, 127, 0 };
}

Layer::Layer(const LayerBaseProps& props, const std::string& imgFile): _props(props) {
    if (imgFile.empty()) {
        _stack.push(Op::Rectangle(props.width, props.height, props.fillColor, props.transpColor));
    } else {
        _stack.push(Op::LoadFromFile(imgFile, props.transpColor));
    }
}

Layer::~Layer() {
}

void Layer::applyTurnAngle() {
    //rotate(_img, _rotatedImg, _turnAngle);
    //populatePixelBuffer(_rotatedImg, pixelBuf());
}

void Layer::setTurnAngle(int angle) {
    /*_turnAngle = angle;

    applyTurnAngle();*/
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
    const auto& img = image();

    if (img.cols == 0 && img.rows == 0)
        return;

    draw(hdc, pixelBuf(), /*_xOffset*/ - x, /*_yOffset*/ - y, img.cols, img.rows);
}

const cv::Vec3b& Layer::pixel(int x, int y, bool rotated) const {
    return ::pixel(image(), x, y);
}

const uint8_t *Layer::pixelDirect(int x, int y, bool rotated) const {
    return ::pixelDirect(image(), x, y);
}

void Layer::setPixel(int x, int y, const AbstractColor& clr) {
    int index = static_cast<int>(y * width() + x);
    pixelBuf()[index] = clr;
}

void Layer::setSize(int width, int height, const AbstractColor& transpClr) {
    image().cols = width;
    image().rows = height;

    auto& buf = pixelBuf();

    buf.resize(width * height);

    for (size_t i = 0; i < buf.size(); buf[i++] = transpClr);
}

const AbstractColor Layer::getRgbQuad(int x, int y, bool rotated) {
    return ::getRgbQuad(pixelBuf(), image().cols, x, y);
}

const AbstractColor Layer::getFillColor() const {
    return color2abstractcolor(_stack.top().fill);
}

const AbstractColor Layer::getTransparentColor() const {
    return color2abstractcolor(_stack.top().transparent);
}
