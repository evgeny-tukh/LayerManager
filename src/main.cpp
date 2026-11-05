#include <cstdio>
#include <memory>

#include <Windows.h>
#include <commctrl.h>

#include "resource.h"
#include "win_tools.hpp"
#include "layers.hpp"
#include "img_wnd.hpp"
#include "layer_props_wnd.hpp"
#include "status_bar.hpp"
#include "layer_base_props_editor.hpp"

namespace {
const char *mainWndName = "Layer Manager v1";
const char *plsConfirmText = "Please confirm";
const char *exitAppText = "Exit app?";
const char *layerFilters = "TIFF files (*.tiff;*.tif)\0*.tiff;*.tif\0PNG files (*.png)\0*.png\0All files\0*.*\0";
const char *layerAddStr = "Add Layer";
const char *openProjectStr = "Open Project";
const char *saveProjectStr { "Save Project" };
const char *projectFilters = "QC projects (*.qcp)\0*.qcp\0All files\0*.*\0";
const char *composition = { "Composition" };
const char *projectLoadFail { "Project load failure" };
const char *projectSaveFail { "Project save failure" };
const char *error { "Error" };

const int layerListWidth = 300;
const int layerPropsHeight = 230;

Layers layers;

std::shared_ptr<ImgWnd> imgWnd;
std::shared_ptr<LayerPropsWnd> layerPropsWnd;
std::shared_ptr<StatusBar> statusBar;

using FilterItem = std::pair<std::string, std::string>;
using FilterList = std::vector<FilterItem>;

const FilterList layersFilter {
    { "TIFF files (*.tiff;*.tif)", "*.tiff;*.tif" },
    { "PNG files (*.png)", "*.png" },
    { "All files", "*.*" },
};

const FilterList projectFilter {
    { "QC projects (*.qcp)", "*.qcp" },
    { "All files", "*.*" },
};

std::string buildFilter(const FilterList& filterList) {
    std::stringstream ss;

    for (const auto& item: filterList)
        ss << item.first << '\0' << item.second << '\0';
    
    ss << '\0';

    return ss.str();
}

bool confirm(HWND wnd, const char *text) {
    return MessageBoxA(wnd, text, plsConfirmText, MB_ICONQUESTION | MB_YESNO) == IDYES;
}

bool confirmExit(HWND wnd) {
    return confirm(wnd, exitAppText);
}

void getLayerName(HWND wnd, int index, char *buf) {
    SendDlgItemMessageA(wnd, ID_LAYERS, LB_GETTEXT, index, reinterpret_cast<LPARAM>(buf));
}

void removeLayerItem(HWND wnd, int index) {
    SendDlgItemMessageA(wnd, ID_LAYERS, LB_DELETESTRING, index, 0);
}

void selectLayerItem(HWND wnd, int index) {
    SendDlgItemMessageA(wnd, ID_LAYERS, LB_SETCURSEL, index, 0);
}

}

namespace MainWnd {

const char *clsName = "LMMainWnd";

int getSelectedLayer(HWND wnd) {
    return static_cast<int>(SendDlgItemMessageA(wnd, ID_LAYERS, LB_GETCURSEL, 0, 0));
}

int getNumOfLayers(HWND wnd) {
    return static_cast<int>(SendDlgItemMessageA(wnd, ID_LAYERS, LB_GETCOUNT, 0, 0)) - 1;
}

void onResize(HWND wnd, unsigned width, unsigned height) {
    RECT statusBarClient;
    statusBar->getWindowRect(statusBarClient);

    height -= statusBarClient.bottom - statusBarClient.top;

    MoveWindow(GetDlgItem(wnd, ID_LAYERS), 0, 0, layerListWidth, height - layerPropsHeight, true);
    MoveWindow(GetDlgItem(wnd, ID_PREVIEW), layerListWidth, 0, width - layerListWidth, height, true);
    MoveWindow(GetDlgItem(wnd, ID_LAYER_PROPS), 0, height - layerPropsHeight, layerListWidth, layerPropsHeight, true);

    statusBar->reposition(width, height);
}

void addCompositionString(HWND wnd) {
    SendDlgItemMessageA(wnd, ID_LAYERS, LB_ADDSTRING, 0, reinterpret_cast<WPARAM>(composition));
}

void initWindow(HWND wnd) {
    RECT client;
    auto instance = getInstanceHandle(wnd);

    GetClientRect(wnd, &client);

    CreateWindowA(
        "LISTBOX",
        "",
        WS_VISIBLE|WS_CHILD|WS_BORDER|LBS_NOINTEGRALHEIGHT|LBS_NOTIFY,
        0,
        0,
        layerListWidth,
        client.bottom - layerPropsHeight,
        wnd,
        reinterpret_cast<HMENU>(ID_LAYERS),
        instance,
        nullptr
    );

    addCompositionString(wnd);

    imgWnd = std::make_shared<ImgWnd>(instance, layers);
    imgWnd->create(wnd, ID_PREVIEW, layerListWidth, 0, client.right - layerListWidth, client.bottom, true, WS_HSCROLL|WS_VSCROLL);

    layerPropsWnd = std::make_shared<LayerPropsWnd>(instance, layers);
    layerPropsWnd->create(wnd, ID_LAYER_PROPS, 0, client.bottom - layerPropsHeight, layerListWidth, layerPropsHeight, false);

    statusBar = std::make_shared<StatusBar>(instance);
    statusBar->create(wnd, ID_STATUS);
}

bool processSysCommand(HWND wnd, WPARAM wp, LPARAM lp) {
    if (wp == SC_CLOSE) {
        if (confirmExit(wnd))
            DestroyWindow(wnd);

        return true;
    }

    return false;
}

std::string getOpenFileName(HWND wnd, const char *title, const FilterList& filterList) {
    OPENFILENAMEA ofn;
    const auto filter = buildFilter(filterList);
    char buf[MAX_PATH];

    memset(&ofn, 0, sizeof(ofn));
    memset(buf, 0, sizeof(buf));

    ofn.Flags = OFN_FILEMUSTEXIST|OFN_HIDEREADONLY;
    ofn.hInstance = getInstanceHandle(wnd);
    ofn.hwndOwner = wnd;
    ofn.lpstrFile = buf;
    ofn.lpstrFilter = filter.data();
    ofn.lpstrTitle = title;
    ofn.lStructSize = sizeof(ofn);
    ofn.nMaxFile = sizeof(buf);

    if (GetOpenFileNameA(&ofn))
        return buf;

    return {};
}

std::string getSaveFileName(HWND wnd, const char *title, const FilterList& filterList, const char *defExt = NULL) {
    OPENFILENAMEA ofn;
    const auto filter = buildFilter(filterList);
    char buf[MAX_PATH];

    memset(&ofn, 0, sizeof(ofn));
    memset(buf, 0, sizeof(buf));

    ofn.Flags = OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY;
    ofn.hInstance = getInstanceHandle(wnd);
    ofn.hwndOwner = wnd;
    ofn.lpstrFile = buf;
    ofn.lpstrFilter = filter.data();
    ofn.lpstrTitle = title;
    ofn.lStructSize = sizeof(ofn);
    ofn.nMaxFile = sizeof(buf);
    ofn.lpstrDefExt = defExt;

    if (GetSaveFileNameA(&ofn))
        return buf;

    return {};
}

void updateStep(HWND wnd, size_t step, size_t steps) {
    if (step >= steps) {
        statusBar->setText(1, "");
        return;
    }

    std::stringstream ss;

    ss << step << " of " << steps << " (" << (step * 100 / steps) << "%)";

    statusBar->setText(1, ss.str().c_str());
}

bool openProject(HWND wnd) {
    auto fileName = getOpenFileName(wnd, openProjectStr, projectFilter);

    if (fileName.empty())
        return false;

    SendDlgItemMessageA(wnd, ID_LAYERS, LB_RESETCONTENT, 0, 0);

    statusBar->setText(0, "Loading project...");

    if (layers.loadProject(fileName.c_str(), [&wnd](size_t step, size_t steps) { updateStep(wnd, step, steps); })) {
        for (size_t i = 0; i < layers.size(); ++i) {
            SendDlgItemMessageA(wnd, ID_LAYERS, LB_ADDSTRING, 0, reinterpret_cast<WPARAM>(layers[i].name().c_str()));
        }
    }

    addCompositionString(wnd);

    statusBar->setText(0, "");

    return true;
}

bool saveProject(HWND wnd) {
    auto fileName = getSaveFileName(wnd, saveProjectStr, projectFilter, ".qcp");

    if (fileName.empty())
        return false;

    statusBar->setText(0, "Saving project...");
    layers.saveProject(fileName.c_str(), [&wnd](size_t step, size_t steps) { updateStep(wnd, step, steps); });
    statusBar->setText(0, "");

    return true;
}

void forceImageRedraw(HWND wnd) {
    InvalidateRect(GetDlgItem(wnd, ID_PREVIEW), NULL, TRUE);
}

void addEmptyLayer(HWND wnd) {
    LayerBaseProps props;
    
    if (editLayerBaseProps(wnd, props)) {
        int count = static_cast<int>(SendDlgItemMessageA(wnd, ID_LAYERS, LB_GETCOUNT, 0, 0)); 
        int item = static_cast<int>(SendDlgItemMessageA(wnd, ID_LAYERS, LB_INSERTSTRING, count - 1, reinterpret_cast<WPARAM>("<Empty>")));
        selectLayerItem(wnd, item);
        layers.addEmptyLayer(props);
        imgWnd->setSelectedLayer(item);
        layerPropsWnd->show();
        layerPropsWnd->setLayerIndex(item);
        forceImageRedraw(wnd);
    }
}

void addLayer(HWND wnd) {
    auto fileName = getOpenFileName(wnd, layerAddStr, layersFilter);

    if (!fileName.empty()) {
        int count = static_cast<int>(SendDlgItemMessageA(wnd, ID_LAYERS, LB_GETCOUNT, 0, 0)); 
        int item = static_cast<int>(SendDlgItemMessageA(wnd, ID_LAYERS, LB_INSERTSTRING, count - 1, reinterpret_cast<WPARAM>(fileName.c_str())));
        selectLayerItem(wnd, item);
        layers.addLayer(fileName.c_str());
        imgWnd->setSelectedLayer(item);
        layerPropsWnd->show();
        layerPropsWnd->setLayerIndex(item);
        forceImageRedraw(wnd);
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
        layers.erase(selection);
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
    SendDlgItemMessageA(wnd, ID_LAYERS, LB_INSERTSTRING, newIndex, reinterpret_cast<LPARAM>(layerName));
    layers.swapItems(selection, direction);
    selectLayerItem(wnd, newIndex);
}

void onSelectLayer(HWND wnd) {
    int selection = getSelectedLayer(wnd);
    int numOfLayers = getNumOfLayers(wnd);

    if (selection == numOfLayers) {
        layerPropsWnd->hide();
        imgWnd->setSelectedLayer(ImgWnd::Constants::COMPOSITION);
        forceImageRedraw(wnd);
    } else if (selection >= 0 && selection < numOfLayers) {
        imgWnd->setSelectedLayer(selection);
        layerPropsWnd->show();
        layerPropsWnd->setLayerIndex(selection);
        forceImageRedraw(wnd);
    }
}

void compose(HWND wnd) {
    statusBar->setText(0, "Composing...");

    layers.compose2([&wnd](size_t step, size_t steps) { updateStep(wnd, step, steps); });
    imgWnd->setSelectedLayer(ImgWnd::COMPOSITION);
    layerPropsWnd->hide();
    forceImageRedraw(wnd);

    statusBar->setText(0, "");
}

void processCommand(HWND wnd, WPARAM wp, LPARAM lp) {
    switch (LOWORD(wp)) {
        case ID_EXIT:
            if (confirmExit(wnd))
                DestroyWindow(wnd);

            break;

        case ID_OPEN:
            if (!openProject(wnd))
                MessageBoxA(wnd, projectLoadFail, error, MB_ICONEXCLAMATION);
            
            break;

        case ID_SAVE:
            if (!saveProject(wnd))
                MessageBoxA(wnd, projectSaveFail, error, MB_ICONEXCLAMATION);
            
            break;

        case ID_ADD:
            addLayer(wnd);
            break;

        case ID_ADD_EMPTY:
            addEmptyLayer(wnd);
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

        case ID_COMPOSE:
            compose(wnd);
            break;

        case ID_LAYERS:
            if (HIWORD(wp) == LBN_SELCHANGE)
                onSelectLayer(wnd);

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

    INITCOMMONCONTROLSEX data;

    data.dwSize = sizeof(data);
    data.dwICC = ICC_WIN95_CLASSES;

    InitCommonControlsEx(&data);

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