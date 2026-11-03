#pragma once

#include <opencv.hpp>

#include <vector>
#include <string>
#include <optional>
#include <functional>

#include "layer.hpp"

class Layers {
    public:
        using Cb = std::function<void(size_t, size_t)>;

    public:
        Layers();

        void addLayer(const char *file);

        const std::string& name(size_t index) const;
        const cv::Mat& image(size_t index) const;
        void getPixel(size_t index, int x, int y, cv::Vec3b& result) const;
        const size_t width(size_t index) const;
        const size_t height(size_t index) const;

        size_t size() const { return _container.size(); }

        void erase(size_t index);
        void swapItems(size_t index, int direction);

        Layer& operator[](size_t index) { return _container[index]; }

        void compose();
        void compose2(Cb cb = {});

        bool loadProject(const char *file, Cb cb = {});

        auto& begin() { return _container.begin(); }
        auto& end() { return _container.end(); }
        auto& cbegin() { return _container.cbegin(); }
        auto& cend() { return _container.cend(); }

        const RGBQUAD *composition() const { return _composition.data(); }
        int compositionWidth() const { return _compositionWidth; }
        int compositionHeight() const { return _compositionHeight; }

        void drawComposition(HDC hdc, int x, int y);
        bool hasComposition() { return !_composition.empty(); }

    private:
        std::vector<Layer> _container;
        Layer::PixelBuf _composition;
        int _compositionWidth;
        int _compositionHeight;

    private:
        void findMaxDims();
};

bool loadTiff(const char *file);
