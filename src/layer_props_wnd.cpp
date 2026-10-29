#include "layer_props_wnd.hpp"

namespace {
const char *clsName = "LMLayerPropWnd";
}

LayerPropsWnd::LayerPropsWnd(HINSTANCE instance, Layer& layer): GenericWnd(instance, clsName), _layer(layer) {

}

bool LayerPropsWnd::processMessage(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) {
    return false;
}

void LayerPropsWnd::onCreate(HWND wnd, WPARAM wp, LPARAM lp) {

}