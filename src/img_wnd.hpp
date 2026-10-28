#pragma once

#include <Windows.h>

#include "layers.hpp"

class ImgWnd {
    public:
        ImgWnd(HINSTANCE instance, Layers& layers);

        void move(int x, int y, int width, int height);
        void paint();

        HWND operator()() const { return _wnd; }

        void setSelectedLayer(int layer) { _selectedLayer = layer; }

        void create(HWND parent, int x, int y, int width, int height);

    private:
        HINSTANCE _instance;
        HWND _wnd;
        Layers& _layers;
        int _selectedLayer;
};