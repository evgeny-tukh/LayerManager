#include "resource.h"
#include "layer_props_wnd.hpp"

namespace {
const char *clsName = "LMLayerPropWnd";
}

LayerPropsWnd::LayerPropsWnd(HINSTANCE instance, Layers& layers):
    GenericWnd(instance, clsName),
    _layers(layers),
    _selection(-1),
    _red(0),
    _green(0),
    _blue(0) {
    memset(_customColors, 0, sizeof(_customColors));
}

void LayerPropsWnd::selectColor(HWND wnd) {
    CHOOSECOLORA data;
    
    memset(&data, 0, sizeof(data));

    _red = GetDlgItemInt(wnd, ID_RED, NULL, FALSE);
    _green = GetDlgItemInt(wnd, ID_GREEN, NULL, FALSE);
    _blue = GetDlgItemInt(wnd, ID_BLUE, NULL, FALSE);

    data.lStructSize = sizeof(data);
    data.hwndOwner = wnd;
    data.rgbResult = RGB(_red, _green, _blue);
    data.Flags = CC_ANYCOLOR|CC_RGBINIT|CC_SOLIDCOLOR|CC_FULLOPEN;
    data.lpCustColors = _customColors;

    if (ChooseColorA(&data)) {
        _red = GetRValue(data.rgbResult);
        _green = GetGValue(data.rgbResult);
        _blue = GetBValue(data.rgbResult);

        SetDlgItemInt(wnd, ID_RED, _red, FALSE);
        SetDlgItemInt(wnd, ID_GREEN, _green, FALSE);
        SetDlgItemInt(wnd, ID_BLUE, _blue, FALSE);
    }
}

void LayerPropsWnd::processCommand(HWND wnd, UINT cmd) {
    switch (cmd) {
        case ID_SELECT_COLOR:
            selectColor(wnd);
            break;
    }
}

bool LayerPropsWnd::processMessage(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_COMMAND) {
        processCommand(wnd, LOWORD(wp));
        return true;
    }

    return false;
}

void LayerPropsWnd::onCreate(HWND wnd, WPARAM wp, LPARAM lp) {
    createLabel("X offset", 10, 10, 70, 20);
    createLabel("Y offset", 10, 40, 70, 20);
    createLabel("Turn angle", 10, 70, 70, 20);
    createLabel("Fill red", 10, 100, 70, 20);
    createLabel("Fill green", 10, 130, 70, 20);
    createLabel("Fill blue", 10, 160, 70, 20);

    createEditor(ID_X_OFFSET, 90, 10, 50, 20, "0");
    createEditor(ID_Y_OFFSET, 90, 40, 50, 20, "0");
    createEditor(ID_ANGLE, 90, 70, 50, 20, "0");
    createEditor(ID_RED, 90, 100, 50, 20, std::to_string(_red).c_str());
    createEditor(ID_GREEN, 90, 130, 50, 20, std::to_string(_green).c_str());
    createEditor(ID_BLUE, 90, 160, 50, 20, std::to_string(_blue).c_str());

    createImage(ID_COLOR_BOX, 150, 100, 80, 80);
    createButton(ID_SELECT_COLOR, 240, 100, 50, 80, "...");
    createButton(ID_APPLY, 150, 190, 140, 30, "Apply...");
}