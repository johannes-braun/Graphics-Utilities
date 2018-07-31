#include <Windows.h>
#include <functional>
#include <string>

namespace gfx {
inline namespace v1 {
namespace win32 {
class window
{
public:
    window();

    HWND _hnd;
};

LRESULT CALLBACK window_callback(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

window::window()
{
    const HINSTANCE instance = GetModuleHandle(nullptr);

    WNDCLASSW wc{0};
    wc.lpfnWndProc   = window_callback;
    wc.hInstance     = instance;
    wc.hbrBackground = HBRUSH(COLOR_BACKGROUND);
    wc.lpszClassName = L"gfx::win32::window";
    wc.style         = CS_OWNDC;
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);

    if (!RegisterClassW(&wc)) throw "up";

    auto flags = WS_OVERLAPPED | WS_VISIBLE | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME | WS_MAXIMIZEBOX;
    ShowWindow(GetConsoleWindow(), false);
    _hnd = CreateWindowW(L"gfx::win32::window", L"My Window", flags, 50, 50, 1280, 720, nullptr, nullptr, instance, nullptr);
}

bool checkbox(HMENU menu, UINT menu_item)
{
    MENUITEMINFO menuItem{0};
    menuItem.cbSize = sizeof(MENUITEMINFO);
    menuItem.fMask  = MIIM_STATE;

    GetMenuItemInfo(menu, menu_item, FALSE, &menuItem);
    if (menuItem.fState == MFS_CHECKED) {
        // Checked, uncheck it
        menuItem.fState = MFS_UNCHECKED;
    }
    else
    {
        // Unchecked, check it
        menuItem.fState = MFS_CHECKED;
    }
    SetMenuItemInfo(menu, menu_item, FALSE, &menuItem);
    return menuItem.fState == MFS_CHECKED;
}

class menu;
using callback  = std::function<void()>;
using bcallback = std::function<void(bool)>;
class menu_bar
{
public:
    menu_bar(HWND window) : _window(window), _menu(CreateMenu()) { SetMenu(window, _menu); }
    menu_bar& item(const menu& submenu);

    void poll(uint32_t msg, uint64_t data)
    {
        if (msg == WM_COMMAND) {
            if (const auto it = _callbacks.find(uint32_t(data)); it != _callbacks.end())
                it->second();
            else if (const auto it = _bcallbacks.find(uint32_t(data)); it != _bcallbacks.end())
                it->second(win32::checkbox(_menu, it->first));
        }
    }

private:
    HWND                                    _window;
    HMENU                                   _menu;
    std::unordered_map<uint32_t, callback>  _callbacks;
    std::unordered_map<uint32_t, bcallback> _bcallbacks;
};

class menu
{
public:
    friend menu_bar;
    menu(const std::string& name) : _menu(CreatePopupMenu()), _name(name) {}

    menu& popup(const menu& submenu)
    {
        AppendMenu(_menu, MF_STRING | MF_POPUP, reinterpret_cast<UINT>(submenu._menu), submenu._name.c_str());

        for (const auto cb : submenu._callbacks) _callbacks[cb.first] = cb.second;
        for (const auto cb : submenu._bcallbacks) _bcallbacks[cb.first] = cb.second;
        return *this;
    }
    menu& button(const std::string& name, callback cb)
    {
        auto h        = h32(name);
        _callbacks[h] = cb;
        AppendMenu(_menu, MF_STRING, h, name.c_str());
        return *this;
    }
    menu& checkbox(const std::string& name, bool checked, bcallback bcb)
    {
        auto h         = h32(name);
        _bcallbacks[h] = bcb;
        AppendMenu(_menu, MF_STRING | (checked ? MF_CHECKED : 0), h, name.c_str());
        return *this;
    }
    menu& separator()
    {
        AppendMenu(_menu, MF_SEPARATOR, 0, "");
        return *this;
    }

private:
    static uint32_t h32(const std::string& s)
    {
        auto h = std::hash<std::string>()(s);
        return static_cast<uint32_t>(h ^ (h >> 32ull));
    }
    HMENU                                     _menu;
    std::unordered_map<std::string, uint32_t> _ids;
    std::unordered_map<uint32_t, callback>    _callbacks;
    std::unordered_map<uint32_t, bcallback>   _bcallbacks;
    std::string                               _name;
};

menu_bar& menu_bar::item(const menu& submenu)
{
    AppendMenu(_menu, MF_STRING | MF_POPUP, reinterpret_cast<UINT>(submenu._menu), submenu._name.c_str());
    SetMenu(_window, _menu);
    for (const auto cb : submenu._callbacks) _callbacks[cb.first] = cb.second;
    for (const auto cb : submenu._bcallbacks) _bcallbacks[cb.first] = cb.second;
    return *this;
}

std::shared_ptr<gfx::win32::menu_bar> main_menu;
LRESULT CALLBACK window_callback(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
    constexpr UINT item_console = 0;
    constexpr UINT item_exit    = 1;

    switch (message)
    {
    case WM_CREATE:
    {
        main_menu = std::make_shared<menu_bar>(window);
        main_menu
            ->item(gfx::win32::menu("File")
                       .button("Open...", [] {})
                       .button("Save...", [] {})
                       .separator()
                       .button("Close", [] { exit(1); })
                       .button("Exit", [] { exit(0); }))
            .item(gfx::win32::menu("Edit")
                      .button("Copy", [] {})
                      .button("Paste", [] {})
                      .separator()
                      .popup(gfx::win32::menu("Tools")
                                 .checkbox("Show Console", true, [](bool b) { ShowWindow(GetConsoleWindow(), b); })
                                 .button("Exit", [] {})
                                 .button("Advanced", [] {}))
                      .button("Properties", [] {}));
    }
    break;
        break;
    case WM_DESTROY: { exit(0);
    }
    break;
    }
    main_menu->poll(message, wparam);
    return DefWindowProcW(window, message, wparam, lparam);
}
}    // namespace win32
}    // namespace v1
}    // namespace gfx

int main()
{
    gfx::win32::window win;

    while (true) {
        MSG msg{0};
        if (PeekMessage(&msg, win._hnd, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}