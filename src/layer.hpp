#pragma once

#include <string>
#include <vector>

#include <opencv.hpp>
#include <Windows.h>

class Layer {
    public:
        using PixelBuf = std::vector<RGBQUAD>;

    public:
        Layer() {}
        Layer(const char *imgFile);
        Layer(
            const char *file,
            int xOffset,
            int yOffset,
            RGBQUAD fillColor,
            int turnAngle
        );

        ~Layer();

        void draw(HDC hdc);
        static void draw(HDC hdc, const PixelBuf& pixelBuf, int x, int y, int width, int height);

        const std::string& name() const { return _imgFile; }
        const cv::Mat& image() const { return _img; }
        const cv::Vec3b& pixel(int x, int y, bool rotated) const;
        const uint8_t *pixelDirect(int x, int y, bool rotated) const;
        const size_t width() const { return _img.cols; }
        const size_t height() const { return _img.rows; }

        void setSize(int width, int height, RGBQUAD transpClr);
        void setPixel(int x, int y, RGBQUAD clr);

        void getOffset(int& xOffset, int& yOffset) const;
        void setOffset(int xOffset, int yOffset);

        void setFillColor(RGBQUAD color) { _fillColor = color; }
        RGBQUAD getFillColor() const { return _fillColor; }

        RGBQUAD getRgbQuad(int x, int y, bool rotated);

        int getTurnAngle() const { return _turnAngle; }
        void setTurnAngle(int angle);

        void applyTurnAngle();

        bool showRotated(bool flag) { _showRotated = flag; return flag; }
        bool showRotated() { return _showRotated; }

        int xOffset() const { return _xOffset; }
        int yOffset() const { return _yOffset; }

    private:
        RGBQUAD _fillColor;
        std::string _imgFile;
        cv::Mat _img;
        cv::Mat _rotatedImg;
        int _xOffset;
        int _yOffset;
        int _turnAngle;
        PixelBuf _pixelBuffer;
        PixelBuf _rotatedPixelBuffer;
        bool _showRotated;

    private:
        void initImage(const char *image);
};