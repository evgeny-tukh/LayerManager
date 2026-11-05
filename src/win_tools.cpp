#include <Windows.h>

#include "defs.hpp"

HINSTANCE getInstanceHandle(HWND wnd) {
    return reinterpret_cast<HINSTANCE>(GetWindowLongPtr(wnd, GWLP_HINSTANCE));
}

RGBQUAD color2rgbquad(const AbstractColor& color) {
    RGBQUAD result;

    result.rgbRed = color.red;
    result.rgbBlue = color.blue;
    result.rgbGreen = color.green;

    return result;
}

COLORREF color2colorref(const AbstractColor& color) {
    return RGB(color.red, color.green, color.blue);
}
