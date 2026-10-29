#include "generic_wnd.hpp"

#include "resource.h"
#include "img_wnd.hpp"

namespace {

LRESULT wndProc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_CREATE: {
            auto cs = reinterpret_cast<CREATESTRUCTA *>(lp);

            SetWindowLongPtr(wnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(cs->lpCreateParams));

            reinterpret_cast<GenericWnd *>(cs->lpCreateParams)->onCreate(wnd, wp, lp);

            return 0;
        }

        default: {
            auto self = GenericWnd::getSelf(wnd);

            if (self && self->processMessage(wnd, msg, wp, lp))
                return 0;
        }
    }

    return DefWindowProcA(wnd, msg, wp, lp);
}

}

GenericWnd::GenericWnd(HINSTANCE instance, const char *clsName): _wnd(NULL), _instance(instance), _clsName(clsName) {
    WNDCLASSA cls;

    memset(&cls, 0, sizeof(cls));

    cls.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(GRAY_BRUSH));
    cls.hCursor = LoadCursor(instance, IDC_ARROW);
    cls.hIcon = LoadIcon(instance, IDI_APPLICATION);
    cls.hInstance = instance;
    cls.lpfnWndProc = wndProc;
    cls.lpszClassName = clsName;
    cls.lpszMenuName = nullptr;
    cls.style = CS_VREDRAW | CS_HREDRAW;

    RegisterClassA(&cls);
}

GenericWnd *GenericWnd::getSelf() {
    return reinterpret_cast<ImgWnd *>(GetWindowLongPtr(_wnd, GWLP_USERDATA));
}

GenericWnd *GenericWnd::getSelf(HWND wnd) {
    return reinterpret_cast<ImgWnd *>(GetWindowLongPtr(wnd, GWLP_USERDATA));
}

void GenericWnd::create(HWND parent, int x, int y, int width, int height) {
    _wnd = CreateWindowA(_clsName.c_str(), "", WS_CHILD|WS_VISIBLE, x, y, width, height, parent, reinterpret_cast<HMENU>(ID_PREVIEW), _instance, this);

    ShowWindow(_wnd, SW_SHOW);
    UpdateWindow(_wnd);
}

void GenericWnd::move(int x, int y, int width, int height) {
    MoveWindow(_wnd, x, y, width, height, TRUE);
}

