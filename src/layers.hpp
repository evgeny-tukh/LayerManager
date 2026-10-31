#pragma once

#include <opencv.hpp>

#include <vector>
#include <string>
#include <optional>

#include "layer.hpp"

class Layers {
    public:
        Layers();

        void addLayer(const char *file);

        const std::string& name(size_t index) const;
        const cv::Mat& image(size_t index) const;
        const cv::Vec3b& pixel(size_t index, int x, int y) const;
        const size_t width(size_t index) const;
        const size_t height(size_t index) const;

        void drawAt(HDC ctx, size_t index);
        void drawAt(HWND wnd, size_t index);

        void drawAtFast(HWND wnd, size_t index);

        size_t size() const { return _container.size(); }

        void erase(size_t index);
        void swapItems(size_t index, int direction);

        Layer& operator[](size_t index) { return _container[index]; }

        void compose();

        bool loadProject(const char *file);

        auto& begin() { return _container.begin(); }
        auto& end() { return _container.end(); }
        auto& cbegin() { return _container.cbegin(); }
        auto& cend() { return _container.cend(); }

        const RGBQUAD *composition() const { return _composition.data(); }
        int compositionWidth() const { return _compositionWidth; }
        int compositionHeight() const { return _compositionHeight; }

        void drawComposition(HDC hdc);
        bool hasComposition() { return !_composition.empty(); }

    private:
        std::vector<Layer> _container;
        Layer::PixelBuf _composition;
        int _compositionWidth;
        int _compositionHeight;
};

bool loadTiff(const char *file);
