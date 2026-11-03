#include <cstdint>
#include <tuple>

#include <Windows.h>

#include <opencv.hpp>

#include "layers.hpp"
#include "json_lite.h"

namespace {
const std::string emptyStr;
const cv::Mat emptyMatrix;
const cv::Vec3b emptyPixel;
const RGBQUAD transpColor { 255, 255, 255, 0 };
const RGBQUAD zeroColor { 0, 0, 0, 0 };

RGBQUAD readColor(json::hashNode *node) {
    RGBQUAD result;

    if (!node || node == json::nothing)
        return zeroColor;

    result.rgbRed = json::childNumericValueOr(node, "red", 0);
    result.rgbGreen = json::childNumericValueOr(node, "green", 0);
    result.rgbBlue = json::childNumericValueOr(node, "blue", 0);

    return result;
}

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

Layers::Layers(): _compositionWidth(0), _compositionHeight(0) {
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
        return emptyMatrix;

    return _container[index].image();
}

void Layers::getPixel(size_t index, int x, int y, cv::Vec3b& result) const {
    if (index >= _container.size())
        result = emptyPixel;
    else
        result = _container[index].image();
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

void Layers::findMaxDims() {
    _compositionWidth = 0;
    _compositionHeight = 0;

    for (const auto& layer: _container) {
        if (layer.width() > _compositionWidth)
            _compositionWidth = static_cast<int>(layer.width());

        if (layer.height() > _compositionHeight)
            _compositionHeight = static_cast<int>(layer.height());
    }

    _composition.resize(_compositionWidth * _compositionHeight);
}

void Layers::compose() {
    findMaxDims();

    for (int x = 0; x < _compositionWidth; ++x) {
        for (int y = 0; y < _compositionHeight; ++y) {
            size_t index = x + y * _compositionWidth;
                    
            for (auto& layer: _container) {
                int srcX = x - layer.xOffset();
                int srcY = y - layer.yOffset();

                if (srcX < 0 || srcX >= layer.width() || srcY < 0 || srcY >= layer.height())
                    continue;

                bool useRotation = layer.getTurnAngle() != 0;
                auto pix = layer.getRgbQuad(srcX, srcY, useRotation);

                if (transpColor.rgbBlue != pix.rgbBlue || transpColor.rgbGreen != pix.rgbGreen || transpColor.rgbRed != pix.rgbRed)
                    _composition[index] = layer.getFillColor();
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

bool Layers::loadProject(const char *file, Cb cb) {
    FILE *project = fopen(file, "r");

    if (!project)
        return false;

    char ch;
    std::stringstream ss;

    while (fread(&ch, 1, 1, project) > 0)
        ss << ch;

    fclose(project);

    int curPos = 0;
    auto json = json::parse(ss.str(), curPos);

    if (!json || json == json::nothing || json->type != json::nodeType::hash)
        return false;

    auto root = reinterpret_cast<json::hashNode *>(json);

    if (!root) {
        delete json;
        return false;
    }

    auto transpColor = readColor(json::childAsHashNodeOrNull(root, "transparentColor"));
    auto layersNode = json::childAsArrayNodeOrNull(root, "layers");

    _container.clear();

    size_t numOfSteps = layersNode->size();
    size_t step = 0;

    for (auto& layerNode: *layersNode) {
        if (cb)
            cb(step++, numOfSteps);

        std::string file = json::childStringValueOr(layerNode, "file");
        int xOffset = json::childNumericValueOr(layerNode, "xOffset", 0);
        int yOffset = json::childNumericValueOr(layerNode, "yOffset", 0);
        int turnAngle = json::childNumericValueOr(layerNode, "turnAngle", 0);
        auto fillColor = readColor(json::childAsHashNodeOrNull(layerNode, "fillColor"));

        _container.emplace_back(file.c_str(), xOffset, yOffset, fillColor, turnAngle);
    }

    cb(numOfSteps, numOfSteps);

    delete json;

    return true;
}

void Layers::drawComposition(HDC hdc, int x, int y) {
    Layer::draw(hdc, _composition, -x, -y, _compositionWidth, _compositionHeight);
}

void Layers::compose2(Cb cb) {
    findMaxDims();

    // Base layer
    cv::Mat canvas = _container[0].image().clone();

    // Iterate other layers
    const cv::Scalar& transparentColor { 255, 255, 255 };

    size_t numOfSteps = _container.size();

    cb(0, numOfSteps);

    for (size_t i = 1; i < _container.size(); ++i) {
        if (cb)
            cb(i, numOfSteps);
        
        const auto& layer = _container[i].image();

        cv::Mat transparentMask;
        cv::inRange(layer, transparentColor, transparentColor, transparentMask);

        cv::Mat opaqueMask;
        cv::bitwise_not(transparentMask, opaqueMask);

        layer.copyTo(canvas, opaqueMask);
    }

    if (cb)
        cb(numOfSteps - 1, numOfSteps);

    _composition.resize(_compositionWidth * _compositionHeight);

    memcpy(_composition.data(), canvas.data, _composition.size() * sizeof(RGBQUAD));

    if (cb)
        cb(numOfSteps, numOfSteps);
}