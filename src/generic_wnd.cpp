#include "generic_wnd.hpp"

#include "resource.h"
#include "img_wnd.hpp"

namespace {

const char *clsStatiс = "STATIC";
const char *clsEdit = "EDIT";
const char *clsButton = "BUTTON";

LRESULT wndProc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_CREATE: {
            auto cs = reinterpret_cast<CREATESTRUCTA *>(lp);

            SetWindowLongPtr(wnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(cs->lpCreateParams));

            auto self = reinterpret_cast<GenericWnd *>(cs->lpCreateParams);

            self->setHandle(wnd);
            self->onCreate(wnd, wp, lp);

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

    cls.hbrBackground = GetSysColorBrush(COLOR_3DFACE); //reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
    cls.hCursor = LoadCursor(instance, IDC_ARROW);
    cls.hIcon = LoadIcon(instance, IDI_APPLICATION);
    cls.hInstance = instance;
    cls.lpfnWndProc = wndProc;
    cls.lpszClassName = clsName;
    cls.lpszMenuName = nullptr;
    cls.style = CS_VREDRAW | CS_HREDRAW;

    RegisterClassA(&cls);
}

GenericWnd *GenericWnd::getSelf() const {
    return reinterpret_cast<ImgWnd *>(GetWindowLongPtr(_wnd, GWLP_USERDATA));
}

GenericWnd *GenericWnd::getSelf(HWND wnd) {
    return reinterpret_cast<ImgWnd *>(GetWindowLongPtr(wnd, GWLP_USERDATA));
}

void GenericWnd::create(HWND parent, uint64_t id, int x, int y, int width, int height, bool visible) {
    _wnd = CreateWindowA(_clsName.c_str(), "", WS_CHILD|WS_BORDER, x, y, width, height, parent, reinterpret_cast<HMENU>(id), _instance, this);

    if (visible) {
        ShowWindow(_wnd, SW_SHOW);
        UpdateWindow(_wnd);
    }
}

void GenericWnd::move(int x, int y, int width, int height) const {
    MoveWindow(_wnd, x, y, width, height, TRUE);
}

HWND GenericWnd::createChild(const char *clsName, int x, int y, int width, int height, uint64_t id, const char *text, unsigned int style) const {
    return CreateWindowA(clsName, text, WS_CHILD|WS_VISIBLE|style, x, y, width, height, _wnd, reinterpret_cast<HMENU>(id), _instance, NULL);
}

void GenericWnd::createLabel(const char *text, int x, int y, int width, int height) const {
    createChild(clsStatiс, x, y, width, height, IDC_STATIC, text, SS_LEFT);
}

HWND GenericWnd::createImage(uint64_t id, int x, int y, int width, int height) const {
    return createChild(clsStatiс, x, y, width, height, id, "", SS_BITMAP|WS_BORDER);
}

HWND GenericWnd::createEditor(uint64_t id, int x, int y, int width, int height, const char *text) const {
    return createChild(clsEdit, x, y, width, height, id, text, ES_NUMBER|WS_BORDER|ES_CENTER);
}

HWND GenericWnd::createButton(uint64_t id, int x, int y, int width, int height, const char *text, unsigned int style) const {
    return createChild(clsButton, x, y, width, height, id, text, WS_BORDER|style);
}

void GenericWnd::show() const {
    ShowWindow(_wnd, SW_SHOW);
    UpdateWindow(_wnd);
}

void GenericWnd::hide() const {
    ShowWindow(_wnd, SW_HIDE);
}