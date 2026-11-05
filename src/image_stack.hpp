#pragma once

#include <cstdint>
#include <variant>
#include <string>
#include <stack>

#include <opencv.hpp>

#include "defs.hpp"

class ImageStack {
    public:
        enum Operation {
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

            /// @brief Replace all non-transparent colors with a certain fill color
            Fill,

            /// @brief Move the image both across X and Y axes
            Shift,

            /// @brief Turn an image on the certain angle
            Rotate,

            /// @brief Duplicate an image the certain number of times across X and Y axes
            Tile,
        };

        struct Param {
            std::string strValue;
            int intValues[4] { 0, 0, 0, 0 };
        };

        struct Item {
            Color transparent = { 255, 255, 255 };
            Color fill = { 128, 128, 128 };
            Size size { 0, 0 };
            Size shift { 0, 0 };
            Operation oper = Operation::NoOper;
            Param param;
            Image image;
        };

        void push(Operation oper, Param param);
        void pop() { _container.pop(); }

        const auto& top() { return _container.top(); }

        auto size() const { return _container.size(); }

    private:
        std::stack<Item> _container;

    private:
        void resize(int width, int height);
        void rotate(int angle);
        void loadFromFile(const std::string& file);
        void fill(uint32_t clr);
        void setTranparentColor(uint32_t clr);
        void shift(int x, int y);
        void tile(const std::string& file, int x, int y, int width, int height);

        Image getLastImage();

        static void correctColors(Image& image, Color tranparentColor, Color nonTransparentColor);
};
