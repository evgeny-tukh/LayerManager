#pragma once

#include <Windows.h>

#include "layers.hpp"
#include "generic_wnd.hpp"

class LayerPropsWnd: public GenericWnd {
    public:
        LayerPropsWnd(HINSTANCE instance, Layer& layer);

        bool processMessage(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) override;

        void onCreate(HWND wnd, WPARAM wp, LPARAM lp) override;

    private:
        Layer& _layer;
};