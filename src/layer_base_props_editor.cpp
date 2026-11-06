#include <cstdint>

#include <Windows.h>

#include "defs.hpp"
#include "win_tools.hpp"
#include "resource.h"

namespace {

COLORREF customColors[16];
LayerBaseProps *getLayerBaseProps(HWND wnd) {
    return reinterpret_cast<LayerBaseProps *>(GetWindowLongPtr(wnd, GWLP_USERDATA));
}

void paintBitmap(HWND wnd, UINT id, const AbstractColor& clr) {
    auto ctl = GetDlgItem(wnd, id);
    auto paintDC = GetDC(ctl);
    auto brush = CreateSolidBrush(color2colorref(clr));

    RECT client;
    GetClientRect(ctl, &client);

    FillRect(paintDC, &client, brush);
    ReleaseDC(ctl, paintDC);
    DeleteObject(brush);
}


void editColor(HWND wnd, UINT id) {
    CHOOSECOLORA data;
    auto props = getLayerBaseProps(wnd);
    AbstractColor *color;
    UINT redID, blueID, greenID;
    UINT boxID;

    switch (id) {
        case ID_FILL_COLOR:
            color = &props->fillColor;
            redID = ID_RED;
            blueID = ID_BLUE;
            greenID = ID_GREEN;
            boxID = ID_COLOR_BOX;
            break;
        case ID_TRANSP_COLOR:
            color = &props->transpColor;
            redID = ID_RED_TRANSP;
            blueID = ID_BLUE_TRANSP;
            greenID = ID_GREEN_TRANSP;
            boxID = ID_TRANSP_COLOR_BOX;
            break;
        default:
            return;
    }
    
    memset(&data, 0, sizeof(data));

    data.lStructSize = sizeof(data);
    data.hwndOwner = wnd;
    data.rgbResult = RGB(color->red, color->green, color->blue);
    data.Flags = CC_ANYCOLOR|CC_RGBINIT|CC_SOLIDCOLOR|CC_FULLOPEN;
    data.lpCustColors = customColors;

    if (ChooseColorA(&data)) {
        color->red = GetRValue(data.rgbResult);
        color->green = GetGValue(data.rgbResult);
        color->blue = GetBValue(data.rgbResult);

        SetDlgItemInt(wnd, redID, color->red, FALSE);
        SetDlgItemInt(wnd, greenID, color->green, FALSE);
        SetDlgItemInt(wnd, blueID, color->blue, FALSE);
        InvalidateRect(GetDlgItem(wnd, boxID), NULL, TRUE);
    }
}

void getData(HWND wnd) {
    auto props = getLayerBaseProps(wnd);

    props->width = GetDlgItemInt(wnd, ID_WIDTH, NULL, FALSE);
    props->height = GetDlgItemInt(wnd, ID_HEIGHT, NULL, FALSE);
    props->fillColor.red = GetDlgItemInt(wnd, ID_RED, NULL, FALSE);
    props->fillColor.green = GetDlgItemInt(wnd, ID_GREEN, NULL, FALSE);
    props->fillColor.blue = GetDlgItemInt(wnd, ID_BLUE, NULL, FALSE);
    props->transpColor.red = GetDlgItemInt(wnd, ID_RED_TRANSP, NULL, FALSE);
    props->transpColor.green = GetDlgItemInt(wnd, ID_GREEN_TRANSP, NULL, FALSE);
    props->transpColor.blue = GetDlgItemInt(wnd, ID_BLUE_TRANSP, NULL, FALSE);
}

void onCommand(HWND wnd, uint32_t cmd) {
    switch (cmd) {
        case IDOK:
            getData(wnd);
        case IDCANCEL:
            EndDialog(wnd, cmd);
            break;

        case ID_FILL_COLOR:
        case ID_TRANSP_COLOR:
            editColor(wnd, cmd);
            break;
    }
}

void initDlg(HWND wnd, LPARAM lp) {
    SetWindowLongPtr(wnd, GWLP_USERDATA, lp);

    auto props = reinterpret_cast<LayerBaseProps *>(lp);

    SetDlgItemInt(wnd, ID_WIDTH, props->width, FALSE);
    SetDlgItemInt(wnd, ID_HEIGHT, props->height, FALSE);
    SetDlgItemInt(wnd, ID_RED, props->fillColor.red, FALSE);
    SetDlgItemInt(wnd, ID_GREEN, props->fillColor.green, FALSE);
    SetDlgItemInt(wnd, ID_BLUE, props->fillColor.blue, FALSE);
    SetDlgItemInt(wnd, ID_RED_TRANSP, props->transpColor.red, FALSE);
    SetDlgItemInt(wnd, ID_GREEN_TRANSP, props->transpColor.green, FALSE);
    SetDlgItemInt(wnd, ID_BLUE_TRANSP, props->transpColor.blue, FALSE);
}

INT_PTR drawBitmap(HWND wnd, DRAWITEMSTRUCT *drawInfo, const AbstractColor& clr) {
    HBRUSH brush = CreateSolidBrush(color2colorref(clr));
    FillRect(drawInfo->hDC, &drawInfo->rcItem, brush);
    DeleteObject(brush);

    return 1;
}

INT_PTR drawItem(HWND wnd, WPARAM wp, LPARAM lp) {
    auto drawInfo = reinterpret_cast<DRAWITEMSTRUCT *>(lp);
    auto props = getLayerBaseProps(wnd);

    switch (drawInfo->CtlID) {
        case ID_COLOR_BOX:
            return drawBitmap(wnd, drawInfo, props->fillColor);
        case ID_TRANSP_COLOR_BOX:
            return drawBitmap(wnd, drawInfo, props->transpColor);
        default:
            return 0;
    }
}

INT_PTR CALLBACK dlgProc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_DRAWITEM:
            return drawItem(wnd, wp, lp);

        case WM_COMMAND:
            onCommand(wnd, LOWORD(wp));
            break;

        case WM_INITDIALOG:
            initDlg(wnd, lp);
            break;

        default:
            return 0;
    }

    return 1;
}

}

bool editLayerBaseProps(HWND wnd, LayerBaseProps& props) {
    memset(customColors, 0, sizeof(customColors));

    bool result = DialogBoxParamA(getInstanceHandle(wnd), MAKEINTRESOURCEA(IDD_LAYER_BASE_PROPS), wnd, dlgProc, reinterpret_cast<LPARAM>(&props)) == IDOK;

    return result;
}