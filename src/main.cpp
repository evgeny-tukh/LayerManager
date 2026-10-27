#include <cstdio>

#include <Windows.h>

#include "resource.h"

namespace {
const char *mainWndName = "Layer Manager v1";
const char *plsConfirmText = "Please confirm";
const char *exitAppText = "Exit app?";
const char *layerFilters = "TIFF files (*.tiff)\0*.tiff\0All files\0*.*\0";
const char *layerAddStr = "Add Layer";

const int layerListWidth = 300;

HINSTANCE getInstanceHandle(HWND wnd) {
    return reinterpret_cast<HINSTANCE>(GetWindowLongPtr(wnd, GWLP_HINSTANCE));
}

bool confirm(HWND wnd, const char *text) {
    return MessageBoxA(wnd, text, plsConfirmText, MB_ICONQUESTION | MB_YESNO) == IDYES;
}

bool confirmExit(HWND wnd) {
    return confirm(wnd, exitAppText);
}

void getLayerName(HWND wnd, int index, char *buf) {
    SendDlgItemMessageA(wnd, IDC_LAYERS, LB_GETTEXT, index, reinterpret_cast<LPARAM>(buf));
}

void removeLayerItem(HWND wnd, int index) {
    SendDlgItemMessageA(wnd, IDC_LAYERS, LB_DELETESTRING, index, 0);
}

void selectLayerItem(HWND wnd, int index) {
    SendDlgItemMessageA(wnd, IDC_LAYERS, LB_SETCURSEL, index, 0);
}

}

namespace MainWnd {

const char *clsName = "LMMainWnd";

int getSelectedLayer(HWND wnd) {
    return static_cast<int>(SendDlgItemMessageA(wnd, IDC_LAYERS, LB_GETCURSEL, 0, 0));
}

int getNumOfLayers(HWND wnd) {
    return static_cast<int>(SendDlgItemMessageA(wnd, IDC_LAYERS, LB_GETCOUNT, 0, 0));
}

void onResize(HWND wnd, unsigned width, unsigned height) {
    MoveWindow(GetDlgItem(wnd, IDC_LAYERS), 0, 0, layerListWidth, height, true);
    MoveWindow(GetDlgItem(wnd, IDC_PREVIEW), layerListWidth, 0, width - layerListWidth, height, true);
}

void initWindow(HWND wnd) {
    RECT client;
    auto instance = getInstanceHandle(wnd);

    GetClientRect(wnd, &client);

    CreateWindowA(
        "LISTBOX",
        "",
        WS_VISIBLE|WS_CHILD|WS_BORDER|LBS_NOINTEGRALHEIGHT,
        0,
        0,
        layerListWidth,
        client.bottom,
        wnd,
        reinterpret_cast<HMENU>(IDC_LAYERS),
        instance,
        nullptr
    );

    CreateWindowA(
        "STATIC",
        "",
        WS_VISIBLE|WS_CHILD|WS_BORDER|SS_BITMAP,
        layerListWidth,
        0,
        client.right - layerListWidth,
        client.bottom,
        wnd,
        reinterpret_cast<HMENU>(IDC_PREVIEW),
        instance,
        nullptr
    );
}

bool processSysCommand(HWND wnd, WPARAM wp, LPARAM lp) {
    if (wp == SC_CLOSE) {
        if (confirmExit(wnd))
            DestroyWindow(wnd);

        return true;
    }

    return false;
}

void addLayer(HWND wnd) {
    OPENFILENAMEA ofn;
    char buf[MAX_PATH];

    memset(&ofn, 0, sizeof(ofn));
    memset(buf, 0, sizeof(buf));

    ofn.Flags = OFN_FILEMUSTEXIST|OFN_HIDEREADONLY;
    ofn.hInstance = getInstanceHandle(wnd);
    ofn.hwndOwner = wnd;
    ofn.lpstrFile = buf;
    ofn.lpstrFilter = layerFilters;
    ofn.lpstrTitle = layerAddStr;
    ofn.lStructSize = sizeof(ofn);
    ofn.nMaxFile = sizeof(buf);

    if (GetOpenFileNameA(&ofn)) {
        int item = static_cast<int>(SendDlgItemMessageA(wnd, IDC_LAYERS, LB_ADDSTRING, 0, reinterpret_cast<WPARAM>(buf)));
        selectLayerItem(wnd, item);
    }
}

void removeLayer(HWND wnd) {
    int selection = getSelectedLayer(wnd);

    if (selection < 0)
        return;

    char layerName[MAX_PATH];
    getLayerName(wnd, selection, layerName);

    char text[1000];
    sprintf(text, "Remove the layer '%s'?", layerName);

    if (confirm(wnd, text)) {
        removeLayerItem(wnd, selection);
    }
}

void moveLayer(HWND wnd, int direction) {
    int selection = getSelectedLayer(wnd);
    int numOfLayers = getNumOfLayers(wnd);

    if (selection < 0 || direction == 0)
        return;

    if (direction < 0 && selection == 0)
        return;

    if (direction > 0 && selection == (numOfLayers - 1))
        return;

    char layerName[MAX_PATH];
    int newIndex = direction > 0 ? (selection + 1) : (selection - 1);

    getLayerName(wnd, selection, layerName);
    removeLayerItem(wnd, selection);
    SendDlgItemMessageA(wnd, IDC_LAYERS, LB_INSERTSTRING, newIndex, reinterpret_cast<LPARAM>(layerName));
    selectLayerItem(wnd, newIndex);
}

void processCommand(HWND wnd, WPARAM wp, LPARAM lp) {
    switch (wp) {
        case ID_EXIT:
            if (confirmExit(wnd))
                DestroyWindow(wnd);

            break;

        case ID_ADD:
            addLayer(wnd);
            break;

        case ID_REMOVE:
            removeLayer(wnd);
            break;

        case ID_MOVE_UP:
            moveLayer(wnd, -1);
            break;

        case ID_MOVE_DOWN:
            moveLayer(wnd, 1);
            break;
    }
}

LRESULT wndProc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_SIZE:
            onResize(wnd, LOWORD(lp), HIWORD(lp));
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_COMMAND:
            processCommand(wnd, wp, lp);
            return 0;

        case WM_SYSCOMMAND:
            if (processSysCommand(wnd, wp, lp))
                return 0;

            break;

        case WM_CREATE:
            initWindow(wnd);
            return 1;
    }

    return DefWindowProcA(wnd, msg, wp, lp);
}

bool registerCls(HINSTANCE inst) {
    WNDCLASSA cls;

    memset(&cls, 0, sizeof(cls));

    cls.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
    cls.hCursor = LoadCursor(inst, IDC_ARROW);
    cls.hIcon = LoadIcon(inst, IDI_APPLICATION);
    cls.hInstance = inst;
    cls.lpfnWndProc = wndProc;
    cls.lpszClassName = clsName;
    cls.lpszMenuName = nullptr;
    cls.style = CS_VREDRAW | CS_HREDRAW;

    return RegisterClassA(&cls);
}

}

int WinMain(HINSTANCE inst, HINSTANCE prevInst, char *cmdLine, int showCmd) {
    if (!MainWnd::registerCls(inst))
        return 1;

    auto mainMenu = LoadMenuA(inst, MAKEINTRESOURCEA(ID_MAINMENU));

    auto mainWnd = CreateWindowA(
        MainWnd::clsName,
        mainWndName,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_THICKFRAME,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        HWND_DESKTOP,
        mainMenu,
        inst,
        nullptr
    );

    ShowWindow(mainWnd, SW_SHOW);
    UpdateWindow(mainWnd);

    MSG msg;

    while (GetMessageA(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    DestroyMenu(mainMenu);

    return 0;
}