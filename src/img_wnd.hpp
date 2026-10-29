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

        void setSelectedLayer(int layer) { _selectedLayer = layer; }

        bool processMessage(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) override;

    private:
        Layers& _layers;
        int _selectedLayer;
};