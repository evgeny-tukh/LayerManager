#include <unordered_set>

#include "tools.hpp"
#include "image_stack.hpp"

namespace {
// Operations that do not need left operand so could be done when the stack is empty
std::unordered_set<ImageStack::Operation> unaryOperations {
    ImageStack::Operation::Resize,
    ImageStack::Operation::SetFillColor,
    ImageStack::Operation::SetTransparentColor,
    ImageStack::Operation::FromFile,
};

/// @brief Checks if the operartion is unary
/// @param oper operation
/// @return result flag
bool isUnary(ImageStack::Operation oper) {
    return unaryOperations.find(oper) != unaryOperations.end();
}

}

void ImageStack::push(Operation oper, Param param) {
    if (_container.empty() && !isUnary(oper))
        // the container is empty and the operation is not unary
        // Nothing could be done
        return;

    switch (oper) {
        case Operation::Resize:
            resize(param.intValues[0], param.intValues[1]);
            break;
    }
}

Image ImageStack::getLastImage() {
    if (!_container.empty())
        return _container.top().image.clone();

    Image img(getDefSize(), CV_8UC3, defFillColor());

    return img;
}

void ImageStack::resize(int width, int height) {
    Item item;
    item.oper = Operation::Resize;
    item.size = Size(width, height);

    cv::resize(getLastImage(), item.image, item.size);

    _container.push(item);
}

void ImageStack::rotate(int angle) {

}

void ImageStack::loadFromFile(const std::string& file) {

}

void ImageStack::fill(uint32_t clr) {

}

void ImageStack::setTranparentColor(uint32_t clr) {

}

void ImageStack::shift(int x, int y) {

}

void ImageStack::tile(const std::string& file, int x, int y, int width, int height) {

}

void ImageStack::pushBase(const LayerBaseProps& props, const std::string& file) {
    pushBase(abstractColor2color(props.transpColor), abstractColor2color(props.fillColor), props.width, props.height, file);
}

void ImageStack::pushBase(Color transparent, Color fill, int width, int height, const std::string& file) {
    Item item;

    item.oper = Operation::NoOper;
    item.size = Size(height, width);
    item.transparent = transparent;

    if (file.empty())
        item.image = Image(height, width, CV_8UC3, fill);
    else
        item.image = cv::imread(file, cv::IMREAD_COLOR);

    _container.push(item);

    cv::imshow("Заполненное изображение", item.image);
    cv::waitKey(0);
}