#include "resource.h"
#include "img_wnd.hpp"

namespace {
const char *clsName = "LMPreviewWnd";
const int pageSize = 100;
}

bool ImgWnd::processMessage(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_PAINT) {
        auto self = reinterpret_cast<ImgWnd *>(getSelf());

        if (self)
            self->paint();
    } else if (msg == WM_VSCROLL) {
        verticalScroll(LOWORD(wp), HIWORD(wp));
    } else if (msg == WM_HSCROLL) {
        horizontalScroll(LOWORD(wp), HIWORD(wp));
    } else {
        return false;
    }

    return true;
}

ImgWnd::ImgWnd(HINSTANCE instance, Layers& layers): GenericWnd(instance, clsName), _selectedLayer(-1), _layers(layers), _startX(0), _startY(0) {
}

void ImgWnd::paint() {
    PAINTSTRUCT ps;
    HDC paintDC = BeginPaint(_wnd, &ps);

    if (_selectedLayer == COMPOSITION) {
        if (_layers.hasComposition()) {
            _layers.drawComposition(paintDC, _startX, _startY);
        } else {
            FillRect(paintDC, &ps.rcPaint, GetSysColorBrush(COLOR_3DFACE));
        }
    } else if (_selectedLayer >= 0 && _selectedLayer < _layers.size()) {
        _layers[_selectedLayer].draw(paintDC, _startX, _startY);
    }

    EndPaint(_wnd, &ps);
}


void ImgWnd::setScrollInfo(int cmd, int pos, int max) {
    SCROLLINFO si;

    memset(&si, 0, sizeof(si));

    si.cbSize = sizeof(si);
    si.fMask = SIF_POS|SIF_RANGE;
    si.nMax = max;
    si.nPos = pos;

    SetScrollInfo(_wnd, cmd, &si, true);
}

void ImgWnd::configureScroll(int maxX, int maxY) {
    setScrollInfo(SB_HORZ, _startX, maxX);
    setScrollInfo(SB_VERT, _startY, maxY);
}

void ImgWnd::setSelectedLayer(int layer) {
    _selectedLayer = layer;
    _startX = 0;
    _startY = 0;

    if (layer == COMPOSITION) {
        _maxX = _layers.compositionWidth();
        _maxY = _layers.compositionHeight();
    } else {
        const auto& img = _layers[_selectedLayer].image();

        _maxX = img.cols;
        _maxY = img.rows;
    }

    RECT client;
    getClientRect(client);

    configureScroll(_maxX - client.right, _maxY - client.bottom);
}

void ImgWnd::verticalScroll(int cmd, int value) {
    switch (cmd) {
        case SB_THUMBPOSITION:
        case SB_THUMBTRACK:
            _startY = value;
            break;
        case SB_TOP:
            _startY = 0;
            break;
        case SB_LINEDOWN:
            if (_startY < _maxY)
                ++_startY;
            break;
        case SB_LINEUP:
            if (_startY > 0)
                --_startY;
            break;
        case SB_PAGEDOWN:
            _startY += pageSize;
            if (_startY > _maxY)
                _startY = _maxY;
            break;
        case SB_PAGEUP:
            _startY -= pageSize;
            if (_startY < 0)
                _startY = 0;
            break;
        default:
            return;
    }

    setCurScrollPos(SB_VERT, _startY);
}

void ImgWnd::horizontalScroll(int cmd, int value) {
    switch (cmd) {
        case SB_THUMBPOSITION:
        case SB_THUMBTRACK:
            _startX = value;
            break;
        case SB_LEFT:
            _startX = 0;
            break;
        case SB_LINERIGHT:
            if (_startX < _maxX)
                ++_startX;
            break;
        case SB_LINELEFT:
            if (_startX > 0)
                --_startX;
            break;
        case SB_PAGERIGHT:
            _startX += pageSize;
            if (_startX > _maxX)
                _startX = _maxX;
            break;
        case SB_PAGELEFT:
            _startX -= pageSize;
            if (_startX < 0)
                _startX = 0;
            break;
        default:
            return;
    }

    setCurScrollPos(SB_HORZ, _startX);
}

void ImgWnd::setCurScrollPos(int cmd, int value) {
    SCROLLINFO si;
    memset(&si, 0, sizeof(si));
    si.cbSize = sizeof(si);
    si.fMask = SIF_POS;
    si.nPos = value;
    SetScrollInfo(_wnd, cmd, &si, TRUE);
    InvalidateRect(_wnd, NULL, TRUE);
}

void ImgWnd::onSize(int width, int height) {
    configureScroll(_maxX - width, _maxY - height);
}