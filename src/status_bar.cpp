#include <array>

#include <Windows.h>
#include <commctrl.h>

#include "status_bar.hpp"

namespace {
const size_t numOfParts = 3;
}

StatusBar::StatusBar(HINSTANCE instance): GenericWnd(instance, STATUSCLASSNAME, false) {

}

bool StatusBar::processMessage(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) {
    return false;
}

void StatusBar::create(HWND parent, uint64_t id, bool visible) {
    GenericWnd::create(parent, id, 0, 0, 0, 0, visible, SBARS_SIZEGRIP);

    std::array<int, numOfParts> parts;

    RECT client;
    getClientRect(client);
    
    parts[0] = 150;
    parts[1] = 300;
    parts[2] = client.right - parts[1];

    SendMessage(_wnd, SB_SETPARTS, numOfParts, reinterpret_cast<LPARAM>(parts.data()));
}

void StatusBar::reposition(int parentWidth, int parentHeight) {
    RECT client;
    getClientRect(client);
    MoveWindow(_wnd, 0, parentHeight - client.bottom, client.right, client.bottom, TRUE);
}

void StatusBar::setText(int part, const char *text) {
    sendMsg(SB_SETTEXT, part, reinterpret_cast<LPARAM>(text));
}