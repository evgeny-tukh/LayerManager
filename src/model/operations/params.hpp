#pragma once

#include <string>
#include <vector>

namespace Op {

class Params {
    public:
        Params() {}
        Params(const std::vector<int>& values);
        Params(const std::string& value);
        Params(const std::string& value, const std::vector<int>& values);

        const std::vector<int>& intValues() const { return _intValues; }
        int intValue(size_t index) const { return _intValues[index]; }
        const std::string& strValue() const { return _strValue; }

    private:
        std::vector<int> _intValues;
        std::string _strValue;
};

}
