#include <Windows.h>

#include "defs.hpp"

HINSTANCE getInstanceHandle(HWND wnd);

RGBQUAD color2rgbquad(const AbstractColor& color);
COLORREF color2colorref(const AbstractColor& color);