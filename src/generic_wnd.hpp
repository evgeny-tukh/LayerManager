#pragma once

#include <Windows.h>

#include <string>

#include "layers.hpp"

class GenericWnd {
    public:
        GenericWnd(HINSTANCE instance, const char *clsName);

        void move(int x, int y, int width, int height);

        HWND operator()() const { return _wnd; }

        void create(HWND parent, int x, int y, int width, int height);

        virtual void onCreate(HWND wnd, WPARAM wp, LPARAM lp) {}

        static GenericWnd *getSelf(HWND wnd);

        virtual bool processMessage(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) { return false; }

    protected:
        HINSTANCE _instance;
        HWND _wnd;
        std::string _clsName;

    protected:
        GenericWnd *getSelf();
};
