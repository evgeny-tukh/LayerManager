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

void LayerPropsWnd::obtainColorComponents() {
    _red = GetDlgItemInt(_wnd, ID_RED, NULL, FALSE);
    _green = GetDlgItemInt(_wnd, ID_GREEN, NULL, FALSE);
    _blue = GetDlgItemInt(_wnd, ID_BLUE, NULL, FALSE);
}

void LayerPropsWnd::selectColor(HWND wnd) {
    CHOOSECOLORA data;
    
    memset(&data, 0, sizeof(data));

    obtainColorComponents();

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

        showColor();
    }
}

void LayerPropsWnd::processCommand(HWND wnd, UINT cmd) {
    switch (cmd) {
        case ID_APPLY:
            apply();
            break;

        case ID_SELECT_COLOR:
            selectColor(wnd);
            break;

        case ID_RED:
            _red = GetDlgItemInt(_wnd, ID_RED, NULL, FALSE);
            showColor();
            break;
            
        case ID_GREEN:
            _green = GetDlgItemInt(_wnd, ID_GREEN, NULL, FALSE);
            showColor();
            break;
            
        case ID_BLUE:
            _blue = GetDlgItemInt(_wnd, ID_BLUE, NULL, FALSE);
            showColor();
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

    showColor();
}

void LayerPropsWnd::showColor() {
    auto wnd = GetDlgItem(_wnd, ID_COLOR_BOX);
    auto hdc = GetDC(wnd);
    auto brush = CreateSolidBrush(RGB(_red, _green, _blue));
 
    RECT client;
    GetClientRect(wnd, &client);
    FillRect(hdc, &client, brush);
    DeleteObject(brush);
    ReleaseDC(wnd, hdc);
}

void LayerPropsWnd::apply() {
    if (_selection >= 0) {
        obtainColorComponents();

        RGBQUAD fillColor;
        fillColor.rgbBlue = _blue;
        fillColor.rgbGreen = _green;
        fillColor.rgbRed = _red;
        fillColor.rgbReserved = 0;

        Layer& layer = _layers[_selection];

        int offsetX = GetDlgItemInt(_wnd, ID_X_OFFSET, NULL, TRUE);
        int offsetY = GetDlgItemInt(_wnd, ID_Y_OFFSET, NULL, TRUE);
        int turnAngle = GetDlgItemInt(_wnd, ID_ANGLE, NULL, TRUE);

        layer.setFillColor(fillColor);
        layer.setOffset(offsetX, offsetY);
        layer.setTurnAngle(turnAngle);
    }
}

void LayerPropsWnd::setLayerIndex(int index) {
    _selection = index;
    Layer& layer = _layers[_selection];

    auto fillColor = layer.getFillColor();
    _red = fillColor.rgbRed;
    _green = fillColor.rgbGreen;
    _blue = fillColor.rgbBlue;

    int offsetX;
    int offsetY;
    int turnAngle = layer.getTurnAngle();
    layer.getOffset(offsetX, offsetY);

    SetDlgItemInt(_wnd, ID_X_OFFSET, offsetX, TRUE);
    SetDlgItemInt(_wnd, ID_Y_OFFSET, offsetY, TRUE);
    SetDlgItemInt(_wnd, ID_ANGLE, turnAngle, TRUE);
    SetDlgItemInt(_wnd, ID_RED, _red, FALSE);
    SetDlgItemInt(_wnd, ID_GREEN, _green, FALSE);
    SetDlgItemInt(_wnd, ID_BLUE, _blue, FALSE);
}
