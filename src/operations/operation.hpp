#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "stage.hpp"
#include "params.hpp"

enum class Operation {
    /// @brief Dummy operation
    NoOper = 0,

    /// @brief Resize the image
    Resize,

    /// @brief Change a fill color
    SetFillColor,

    /// @brief Change a transparent color
    SetTransparentColor,

    /// @brief Load from file
    FromFile,

    /// @brief Move the image both across X and Y axes
    Shift,

    /// @brief Turn an image on the certain angle
    Rotate,

    /// @brief Duplicate an image the certain number of times across X and Y axes
    Tile,
};

class GenericOperation {
    public:
        GenericOperation();
        GenericOperation(Operation oper);
        GenericOperation(Operation oper, const Params& params);

        virtual const std::string toString() const;

        const Operation oper() const { return _oper; }
        const Params& params() const { return _params; }

        virtual Stage execute(const Stage& source = Stage()) const { return {}; }

    protected:
        Operation _oper;
        Params _params;
};

