#include "resource.h"
#include "img_wnd.hpp"

namespace {
const char *clsName = "LMPreviewWnd";

ImgWnd *getSelf(HWND wnd) {
    return reinterpret_cast<ImgWnd *>(GetWindowLongPtr(wnd, GWLP_USERDATA));
}

LRESULT wndProc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_PAINT: {
            auto self = getSelf(wnd);

            if (self)
                self->paint();

            return 0;
        }

        case WM_CREATE: {
            auto cs = reinterpret_cast<CREATESTRUCTA *>(lp);

            SetWindowLongPtr(wnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(cs->lpCreateParams));
            return 0;
        }
    }

    return DefWindowProcA(wnd, msg, wp, lp);
}
}

ImgWnd::ImgWnd(HINSTANCE instance, Layers& layers): _wnd(NULL), _selectedLayer(-1), _instance(instance), _layers(layers) {
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

void ImgWnd::create(HWND parent, int x, int y, int width, int height) {
    _wnd = CreateWindowA(clsName, "", WS_CHILD|WS_VISIBLE, x, y, width, height, parent, reinterpret_cast<HMENU>(ID_PREVIEW), _instance, this);

    ShowWindow(_wnd, SW_SHOW);
    UpdateWindow(_wnd);
}

void ImgWnd::move(int x, int y, int width, int height) {
    MoveWindow(_wnd, x, y, width, height, TRUE);
}

void ImgWnd::paint() {
    PAINTSTRUCT ps;
    HDC paintDC = BeginPaint(_wnd, &ps);

    if (_selectedLayer == COMPOSITION)
        _layers.composition().draw(paintDC);
    else if (_selectedLayer >= 0 && _selectedLayer < _layers.size())
        _layers[_selectedLayer].draw(paintDC);

    EndPaint(_wnd, &ps);
}
