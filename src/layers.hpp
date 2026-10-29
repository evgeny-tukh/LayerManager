#pragma once

#include <opencv.hpp>

#include <vector>
#include <string>
#include <optional>

#include "layer.hpp"

class Layers {
    public:
        Layers() {}

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

        Layer& composition() { return _composition; }

        void compose();

    private:
        std::vector<Layer> _container;
        Layer _composition;
};

bool loadTiff(const char *file);
