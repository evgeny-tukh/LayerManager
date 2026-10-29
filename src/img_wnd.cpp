#include "resource.h"
#include "img_wnd.hpp"

namespace {
const char *clsName = "LMPreviewWnd";
}

bool ImgWnd::processMessage(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_PAINT) {
        auto self = reinterpret_cast<ImgWnd *>(getSelf());

        if (self)
            self->paint();

        return true;
    }

    return false;
}

ImgWnd::ImgWnd(HINSTANCE instance, Layers& layers): GenericWnd(instance, clsName), _selectedLayer(-1), _layers(layers) {
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
