#pragma once

#include <string>
#include <vector>

#include <opencv.hpp>
#include <Windows.h>

#include "stages_stack.hpp"
#include "tools.hpp"

class Layer {
    public:
        Layer(const LayerBaseProps& props, const std::string& imgFile = {});
        ~Layer();

        void draw(HDC hdc, int x, int y);
        static void draw(HDC hdc, const PixelBuf& pixelBuf, int x, int y, int width, int height);

        const cv::Vec3b& pixel(int x, int y, bool rotated) const;
        const uint8_t *pixelDirect(int x, int y, bool rotated) const;
        const size_t width() const { return image().cols; }
        const size_t height() const { return image().rows; }

        void setSize(int width, int height, const AbstractColor& transpClr);
        void setPixel(int x, int y, const AbstractColor& clr);

        const AbstractColor getFillColor() const;
        const AbstractColor getTransparentColor() const;

        const AbstractColor getRgbQuad(int x, int y, bool rotated);

        void setTurnAngle(int angle);

        void applyTurnAngle();

        PixelBuf& pixelBuf() { return _stack.top().pixelBuf; }
        
        Image& image() { return _stack.top().image; }
        const Image& image() const { return _stack.top().image; }

        const std::string& name() const { return _props.name; }

    private:
        StagesStack _stack;
        LayerBaseProps _props;
};