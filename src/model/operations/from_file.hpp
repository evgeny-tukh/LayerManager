#pragma once

#include "tools.hpp"
#include "operation.hpp"

namespace Op {

class LoadFromFile: public GenericOperation {
    public:
        LoadFromFile(const std::string& file, const AbstractColor& transpColor = defTransparentColorA());

        Stage execute(const Stage& source = Stage()) const override;

        const std::string toString() const override;

    private:
        const std::string& fileName() const { return _params.strValue(); }
        std::string fileName() { return _params.strValue(); }
        const AbstractColor transpColor() const;
};

}
