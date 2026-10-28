#pragma once

#include <string>
#include <vector>

#include <opencv.hpp>
#include <Windows.h>

class Layer {
    public:
        Layer() {}
        Layer(const char *imgFile);
        ~Layer();

        void draw(HDC hdc);

        const std::string& name() const { return _imgFile; }
        const cv::Mat& image() const { return _img; }
        const cv::Vec3b& pixel(int x, int y) const;
        const size_t width() const { return _img.cols; }
        const size_t height() const { return _img.rows; }

        void setSize(int width, int height, RGBQUAD transpClr);
        void setPixel(int x, int y, RGBQUAD clr);

    private:
        std::string _imgFile;
        cv::Mat _img;
        int _xOffset;
        int _yOffset;
        std::vector<RGBQUAD> _pixelBuffer;
};