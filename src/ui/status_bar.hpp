#pragma once

#include <Windows.h>

#include "layers.hpp"
#include "generic_wnd.hpp"

class StatusBar: public GenericWnd {
    public:
        StatusBar(HINSTANCE instance);

        bool processMessage(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) override;

        void create(HWND parent, uint64_t id, bool visible = true);

        void reposition(int parentWidth, int parentHeight);

        void setText(int part, const char *text);
};