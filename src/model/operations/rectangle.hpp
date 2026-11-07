#pragma once

#include "tools.hpp"
#include "operation.hpp"

namespace Op {

class Rectangle: public GenericOperation {
    public:
        Rectangle(int width, int height, const AbstractColor& fillColor = defFillColorA(), const AbstractColor& transpColor = defTransparentColorA());

        Stage execute(const Stage& source = Stage()) const override;

        const std::string toString() const override;

    private:
        int width() const { return _params.intValue(0); }
        int height() const { return _params.intValue(1); }
        const AbstractColor fillColor() const;
        const AbstractColor transpColor() const;
};

}
