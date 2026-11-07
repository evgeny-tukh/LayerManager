#include <string>
#include <unordered_set>
#include <unordered_map>
#include <functional>

#include "tools.hpp"
#include "stages_stack.hpp"

namespace {

// Operations that do not need left operand so could be done when the stack is empty
std::unordered_set<Op::Code> unaryOperations {
    Op::Code::Resize,
    Op::Code::SetFillColor,
    Op::Code::SetTransparentColor,
    Op::Code::FromFile,
};

/// @brief Checks if the operartion is unary
/// @param oper operation
/// @return result flag
bool isUnary(Op::Code oper) {
    return unaryOperations.find(oper) != unaryOperations.end();
}

}

StagesStack::StagesStack() {
}

Image StagesStack::getLastImage() {
    if (!_container.empty())
        return top().image.clone();

    Image img(getDefSize(), CV_8UC3, defFillColor());

    return img;
}

void StagesStack::resize(int width, int height) {
    /*Item item;
    item.oper = Operation::Resize;
    item.size = Size(width, height);

    cv::resize(getLastImage(), item.image, item.size);

    _container.push(item);*/
}

void StagesStack::rotate(int angle) {

}

void StagesStack::loadFromFile(const std::string& file) {

}

void StagesStack::fill(uint32_t clr) {

}

void StagesStack::setTranparentColor(uint32_t clr) {

}

void StagesStack::shift(int x, int y) {

}

void StagesStack::tile(const std::string& file, int x, int y, int width, int height) {

}

void StagesStack::push(Op::GenericOperation& operation) {
    const auto& stage = _container.empty() ? Stage() : top();
    _container.push_back(std::pair<std::string, Stage>(operation.toString(), operation.execute(stage)));
}

void StagesStack::populateItemList(std::vector<std::string>& items) {
    items.clear();

    for (const auto& item: _container)
        items.push_back(item.first);
}
