#pragma once

#include <opencv.hpp>

#include <vector>
#include <string>
#include <optional>

class Layers {
    public:
        using OptImage = std::optional<cv::Mat>;
        using OptPixel = std::optional<cv::Vec3b>;
        using ImgItem = std::pair<std::string, cv::Mat>;

    public:
        void addLayer(const char *file);

        const std::string& name(size_t index) const;
        const OptImage img(size_t index) const;
        const OptPixel pixel(size_t index, int x, int y) const;
        const size_t width(size_t index) const;
        const size_t height(size_t index) const;

        void drawAt(HDC ctx, size_t index);
        void drawAt(HWND wnd, size_t index);

        void drawAtFast(HWND wnd, size_t index);

    private:
        std::vector<ImgItem> _container;
};

bool loadTiff(const char *file);
