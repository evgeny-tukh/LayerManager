#pragma once

#include <Windows.h>

#include <string>

#include "layers.hpp"

class GenericWnd {
    public:
        GenericWnd(HINSTANCE instance, const char *clsName);

        void move(int x, int y, int width, int height) const;

        HWND operator()() const { return _wnd; }

        void create(HWND parent, uint64_t id, int x, int y, int width, int height, bool visible = true);

        virtual void onCreate(HWND wnd, WPARAM wp, LPARAM lp) {}

        void setHandle(HWND wnd) { _wnd = wnd; }

        static GenericWnd *getSelf(HWND wnd);

        virtual bool processMessage(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) { return false; }

        void show() const;
        void hide() const;

    protected:
        HINSTANCE _instance;
        HWND _wnd;
        std::string _clsName;

    protected:
        GenericWnd *getSelf() const;

        void createLabel(const char *text, int x, int y, int width, int height) const;
        HWND createImage(uint64_t id, int x, int y, int width, int height) const;
        HWND createEditor(uint64_t id, int x, int y, int width, int height, const char *text = "") const;
        HWND createButton(uint64_t id, int x, int y, int width, int height, const char *text = "") const;
};
