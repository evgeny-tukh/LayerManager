#pragma once

#include <string>
#include <vector>

#include <opencv.hpp>
#include <Windows.h>

#include "stages_stack.hpp"
#include "tools.hpp"

class Layer {
    public:
        Layer();
        Layer(const LayerBaseProps& props, const std::string& imgFile = {});
        /*Layer(const char *imgFile);
        Layer(
            const char *file,
            int xOffset,
            int yOffset,
            const AbstractColor& fillColor,
            int turnAngle
        );*/

        ~Layer();

        void draw(HDC hdc, int x, int y);
        static void draw(HDC hdc, const PixelBuf& pixelBuf, int x, int y, int width, int height);

        //const std::string& name() const { return _stack.top().params.strValue; }
        //const cv::Mat& image() const { return _img; }
        const cv::Vec3b& pixel(int x, int y, bool rotated) const;
        const uint8_t *pixelDirect(int x, int y, bool rotated) const;
        const size_t width() const { return image().cols; }
        const size_t height() const { return image().rows; }

        void setSize(int width, int height, const AbstractColor& transpClr);
        void setPixel(int x, int y, const AbstractColor& clr);

        //void getOffset(int& xOffset, int& yOffset) const;
        //void setOffset(int xOffset, int yOffset);

        //void setFillColor(const AbstractColor& color) { _fillColor = color; }
        const AbstractColor getFillColor() const;
        const AbstractColor getTransparentColor() const;

        const AbstractColor getRgbQuad(int x, int y, bool rotated);

        //int getTurnAngle() const { return _turnAngle; }
        void setTurnAngle(int angle);

        void applyTurnAngle();

        //bool showRotated(bool flag) { _showRotated = flag; return flag; }
        //bool showRotated() { return _showRotated; }

        //int xOffset() const { return _xOffset; }
        //int yOffset() const { return _yOffset; }

        PixelBuf& pixelBuf() { return _stack.top().pixelBuf; }
        
        Image& image() { return _stack.top().image; }
        const Image& image() const { return _stack.top().image; }

        const std::string& name() const { return _props.name; }

    private:
        //AbstractColor _fillColor;
        //AbstractColor _transpColor;
        //std::string _imgFile;
        //cv::Mat _img;
        //cv::Mat _rotatedImg;
        //int _xOffset;
        //int _yOffset;
        //int _turnAngle;
        //PixelBuf _pixelBuffer;
        //PixelBuf _rotatedPixelBuffer;
        //bool _showRotated;
        StagesStack _stack;
        LayerBaseProps _props;

    private:
        //void initImage(const char *image);
};