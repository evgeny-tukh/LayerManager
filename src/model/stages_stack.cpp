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
/*
void formatGenericOpen (const Param&, std::stringstream&) {
    
}

void formatGenericOpen (const Param& param, std::stringstream& ss) {
}

std::unordered_map<Operation, void(*)(const Param&, std::stringstream)> paramFormatter {
    { Operation::NoOper, {} },
    { Operation::Resize, [](Param param, std::stringstream ss) { ss << param.intValues[0] << '*' << param.intValues[1]; } },
    { Operation::SetFillColor, [](Param param, std::stringstream ss) { ss << param.intValues[0] << '*' << param.intValues[1]; } },
    { Operation::SetTransparentColor, "Set Transparent" },
    { Operation::FromFile, "Load from file" },
    { Operation::Shift, "Shift" },
    { Operation::Rotate, "Rotate" },
    { Operation::Tile, "Tile" },
};
*/
}
/*
void StagesStack::push(Operation oper, Param param) {
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
*/
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
/*
void StagesStack::pushBase(const LayerBaseProps& props, const std::string& file) {
    pushBase(abstractColor2color(props.transpColor), abstractColor2color(props.fillColor), props.width, props.height, file);
}

void StagesStack::pushBase(Color transparent, Color fill, int width, int height, const std::string& file) {
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
*/

void StagesStack::push(Op::GenericOperation& operation) {
    const auto& stage = _container.empty() ? Stage() : top();
    _container.push_back(std::pair<std::string, Stage>(operation.toString(), operation.execute(stage)));
}

void StagesStack::populateItemList(std::vector<std::string>& items) {
    items.clear();

    for (const auto& item: _container)
        items.push_back(item.first);
}
