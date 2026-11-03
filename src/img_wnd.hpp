#pragma once

#include <Windows.h>

#include "layers.hpp"
#include "generic_wnd.hpp"

class ImgWnd: public GenericWnd {
    public:
        enum Constants {
            COMPOSITION = 0xFFFF,
        };

    public:
        ImgWnd(HINSTANCE instance, Layers& layers);

        void paint();

        void setSelectedLayer(int layer);

        bool processMessage(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) override;

    private:
        Layers& _layers;
        int _selectedLayer;
        int _startX;
        int _startY;
        int _maxX;
        int _maxY;

    private:
        void setScrollInfo(int cmd, int pos, int max);

        void verticalScroll(int cmd, int value);
        void horizontalScroll(int cmd, int value);

        void onSize(int width, int height);

        void configureScroll(int maxX, int maxY);
};