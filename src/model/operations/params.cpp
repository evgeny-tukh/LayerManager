#include "params.hpp"

namespace Op {

Params::Params(const std::vector<int>& values) {
    _intValues.insert(_intValues.begin(), values.begin(), values.end());
}

Params::Params(const std::string& value): _strValue(value) {}

Params::Params(const std::string& value, const std::vector<int>& values): _strValue(value) {
    _intValues.insert(_intValues.begin(), values.begin(), values.end());
}

}
