#include <sstream>
#include <unordered_map>

#include "operation.hpp"

namespace {

std::unordered_map<Op::Code, std::string> operationNames {
    { Op::Code::NoOper, "No operation" },
    { Op::Code::Resize, "Resize" },
    { Op::Code::SetFillColor, "Set Fill" },
    { Op::Code::SetTransparentColor, "Set Transparent" },
    { Op::Code::FromFile, "Load from file" },
    { Op::Code::Shift, "Shift" },
    { Op::Code::Rotate, "Rotate" },
    { Op::Code::Tile, "Tile" },
};

const std::string toString(const Op::Code& oper) {
    auto operPos = operationNames.find (oper);

    if (operPos == operationNames.end())
        return {};

    std::stringstream ss;

    ss << operPos->second << ": ";

    return ss.str();
}

}

namespace Op {

GenericOperation::GenericOperation(): _oper(Code::NoOper) {}

GenericOperation::GenericOperation(Code oper): _oper(oper) {}

GenericOperation::GenericOperation(Code oper, const Params& params): _oper(oper), _params(params) {}

const std::string GenericOperation::toString() const {
    return ::toString(_oper);
}

}
