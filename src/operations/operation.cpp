#include <sstream>
#include <unordered_map>

#include "operation.hpp"

namespace {

std::unordered_map<Operation, std::string> operationNames {
    { Operation::NoOper, "No operation" },
    { Operation::Resize, "Resize" },
    { Operation::SetFillColor, "Set Fill" },
    { Operation::SetTransparentColor, "Set Transparent" },
    { Operation::FromFile, "Load from file" },
    { Operation::Shift, "Shift" },
    { Operation::Rotate, "Rotate" },
    { Operation::Tile, "Tile" },
};

const std::string toString(const Operation& oper) {
    auto operPos = operationNames.find (oper);

    if (operPos == operationNames.end())
        return {};

    std::stringstream ss;

    ss << operPos->second << ": ";

    return ss.str();
}

}

GenericOperation::GenericOperation(): _oper(Operation::NoOper) {}

GenericOperation::GenericOperation(Operation oper): _oper(oper) {}

GenericOperation::GenericOperation(Operation oper, const Params& params): _oper(oper), _params(params) {}

const std::string GenericOperation::toString() const {
    return ::toString(_oper);
}

