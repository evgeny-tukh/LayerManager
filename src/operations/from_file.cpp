#include "from_file.hpp"

LoadFromFile::LoadFromFile(const std::string& file, const AbstractColor& transpColor):
    GenericOperation(Operation::FromFile, Params(file, { transpColor.red, transpColor.green, transpColor.blue })) {
}

Stage LoadFromFile::execute(const Stage& source) const {
    Stage result(source);

    result.image = cv::imread(fileName(), cv::IMREAD_COLOR);
    int channels = convertToRgbaWithTransparentColor(result.image, abstractColor2color(transpColor()));
    populatePixelBuffer(result.image, result.pixelBuf, channels);

    return result;
}

const AbstractColor LoadFromFile::transpColor() const {
    AbstractColor result;

    result.red = _params.intValue(0);
    result.green = _params.intValue(1);
    result.blue = _params.intValue(2);

    return result;
}