#include "rectangle.hpp"

namespace {
Op::Params constructParams(int width, int height, const AbstractColor& fillColor, const AbstractColor& transpColor) {
    const std::vector<int> intValues {
        width,
        height,
        static_cast<int>(fillColor.red),
        static_cast<int>(fillColor.green),
        static_cast<int>(fillColor.blue),
        static_cast<int>(transpColor.red),
        static_cast<int>(transpColor.green),
        static_cast<int>(transpColor.blue)
    };

    return Op::Params(intValues);
}
}

namespace Op {

Rectangle::Rectangle(int width, int height, const AbstractColor& fillColor, const AbstractColor& transpColor):
    GenericOperation(
        Code::Rectangle,
        constructParams(width, height, fillColor, transpColor)
    ) {
}

Stage Rectangle::execute(const Stage& source) const {
    Stage result(source);

    result.image = Image(height(), width(), CV_8UC3, abstractColor2color(fillColor()));
    int channels = convertToRgbaWithTransparentColor(result.image, abstractColor2color(transpColor()));
    populatePixelBuffer(result.image, result.pixelBuf, channels);

    return result;
}

const std::string Rectangle::toString() const {
    std::stringstream ss;
    auto fc = fillColor();
    auto tc = transpColor();

    ss << GenericOperation::toString() << "W: " << width() << " H: " << height() << " F: " << fc.toString() << " T: " << tc.toString();

    return ss.str();
}

const AbstractColor Rectangle::fillColor() const {
    return AbstractColor(_params.intValue(2), _params.intValue(3), _params.intValue(4));
}

const AbstractColor Rectangle::transpColor() const {
    return AbstractColor(_params.intValue(5), _params.intValue(6), _params.intValue(7));
}

}
