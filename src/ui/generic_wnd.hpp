#pragma once

#include <Windows.h>

#include <string>
#include <cstdint>

#include "layers.hpp"
#include "resource.h"

class GenericWnd {
    public:
        GenericWnd(HINSTANCE instance, const char *clsName, bool registerClass = true);

        void move(int x, int y, int width, int height) const;

        HWND operator()() const { return _wnd; }
        HWND getParent() const { return GetParent(_wnd); }
        HWND getSibling(unsigned int id) { return GetDlgItem(getParent(), id); }

        void create(HWND parent, uint64_t id, int x, int y, int width, int height, bool visible = true, unsigned int extraStyles = 0);

        virtual void onCreate(HWND wnd, WPARAM wp, LPARAM lp) {}

        void setHandle(HWND wnd) { _wnd = wnd; }

        static GenericWnd *getSelf(HWND wnd);

        virtual bool processMessage(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) { return false; }

        void show() const;
        void hide() const;

        void getClientRect(RECT& rect) { GetClientRect(_wnd, &rect); }
        void getWindowRect(RECT& rect) { GetWindowRect(_wnd, &rect); }

    protected:
        HINSTANCE _instance;
        HWND _wnd;
        std::string _clsName;
        uint64_t _id;

    protected:
        GenericWnd *getSelf() const;

        HWND createChild(const char *clsName, int x, int y, int width, int height, uint64_t id = IDC_STATIC, const char *text = "", unsigned int style = 0) const;

        void createLabel(const char *text, int x, int y, int width, int height) const;
        HWND createImage(uint64_t id, int x, int y, int width, int height) const;
        HWND createEditor(uint64_t id, int x, int y, int width, int height, const char *text = "") const;
        HWND createButton(uint64_t id, int x, int y, int width, int height, const char *text = "", unsigned int = BS_PUSHBUTTON) const;

        LRESULT sendMsg(UINT msg, WPARAM wp, LPARAM lp);
};
