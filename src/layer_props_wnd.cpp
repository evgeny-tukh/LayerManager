#include "resource.h"
#include "layer_props_wnd.hpp"

namespace {
const char *clsName = "LMLayerPropWnd";
}

LayerPropsWnd::LayerPropsWnd(HINSTANCE instance, Layers& layers): GenericWnd(instance, clsName), _layers(layers), _selection(-1) {
}

bool LayerPropsWnd::processMessage(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) {
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
    createEditor(ID_RED, 90, 100, 50, 20, "0");
    createEditor(ID_GREEN, 90, 130, 50, 20, "0");
    createEditor(ID_BLUE, 90, 160, 50, 20, "0");

    createImage(ID_COLOR_BOX, 150, 100, 80, 80);
    createButton(ID_SELECT_COLOR, 240, 100, 50, 80, "...");
    createButton(ID_APPLY, 150, 190, 140, 30, "Apply...");
}