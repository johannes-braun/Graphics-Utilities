#include <Windows.h>
#include <functional>
#include <gfx/type.hpp>
#include <string>
#include <utility>
#include <winuser.h>

namespace gfx {
inline namespace v1 {
namespace win32 {
class menu_bar;
class window
{
public:
    friend class menu_bar;
    window();

    std::shared_ptr<gfx::win32::menu_bar> main_menu;

    void poll() const
    {
        MSG msg{};
        if (PeekMessage(&msg, _hnd, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

private:
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

    const auto flags = WS_OVERLAPPED | WS_VISIBLE | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME | WS_MAXIMIZEBOX;
    ShowWindow(GetConsoleWindow(), false);
    _hnd = CreateWindowW(L"gfx::win32::window", L"My Window", flags, 50, 50, 1280, 720, nullptr, nullptr, instance, this);
}

bool checkbox(HMENU menu, UINT menu_item)
{
    MENUITEMINFO menuItem{0};
    menuItem.cbSize = sizeof(MENUITEMINFO);
    menuItem.fMask  = MIIM_STATE;

    GetMenuItemInfo(menu, menu_item, FALSE, &menuItem);
    if (menuItem.fState == MFS_CHECKED)
    {
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

class menu_bar
{
public:
    template<typename... T>
    using basic_callback = std::function<void(T...)>;

    using callback      = basic_callback<>;
    using cmd_callback  = basic_callback<u32>;
    using bool_callback = basic_callback<bool>;

    menu_bar(const window& w) : _window(w._hnd), _menu(CreateMenu()) { SetMenu(w._hnd, _menu); }
    menu_bar& add_item(const menu& submenu);

    void poll(uint32_t msg, uint64_t data);

    void set_callback(cmd_callback callback) { _command_callback = callback; }

private:
    std::vector<menu>                           _submenus;
    cmd_callback                                _command_callback;
	std::unordered_map<uint32_t, callback>      _callbacks;
	std::unordered_map<uint32_t, bool_callback> _bool_callbacks;
    HWND                                        _window;
    HMENU                                       _menu;
};

class menu
{
public:
    friend menu_bar;
    menu(std::string name) : _menu(CreatePopupMenu()), _name(std::move(name)) {}

    menu& popup(const menu& submenu)
    {
        AppendMenu(_menu, MF_STRING | MF_POPUP, reinterpret_cast<UINT>(submenu._menu), submenu._name.c_str());
        _submenus.push_back(submenu);
        return *this;
    }
    menu& button(const std::string& name, menu_bar::callback cb = [] {})
    {
        auto h        = h32(name);
        _callbacks[h] = cb;
        AppendMenu(_menu, MF_STRING, h, strip(name).c_str());
        return *this;
    }
    menu& checkbox(const std::string& name, bool checked, menu_bar::bool_callback bcb = [](bool) {})
    {
        auto h             = h32(name);
        _bool_callbacks[h] = bcb;
        AppendMenu(_menu, MF_STRING | (checked ? MF_CHECKED : 0), h, strip(name).c_str());
        return *this;
    }
    menu& separator()
    {
        AppendMenu(_menu, MF_SEPARATOR, 0, "");
        return *this;
    }
    void poll(uint32_t msg, uint64_t data)
    {
        if (msg == WM_COMMAND)
        {
            if (const auto it = _callbacks.find(uint32_t(data)); it != _callbacks.end())
                it->second();
            else if (const auto it = _bool_callbacks.find(uint32_t(data)); it != _bool_callbacks.end())
                it->second(win32::checkbox(_menu, it->first));
            for (auto& m : _submenus) m.poll(msg, data);
        }
    }

private:
    static std::string strip(std::string str)
    {
        for (auto i = str.begin(); i != str.end() - 2; ++i)
        {
            if (*i == '#' && *(i + 1) == '#') return std::string(str.begin(), i);
        }
        return str;
    }
    static uint32_t h32(const std::string& s)
    {
        auto h = std::hash<std::string>()(s);
        return static_cast<uint32_t>(h ^ (h >> 32ull));
    }
    HMENU                                                 _menu;
    std::unordered_map<uint32_t, menu_bar::callback>      _callbacks;
    std::unordered_map<uint32_t, menu_bar::bool_callback> _bool_callbacks;
    std::string                                           _name;
    std::vector<menu>                                     _submenus;
};

void menu_bar::poll(uint32_t msg, uint64_t data)
{
    if (msg == WM_COMMAND)
    {
        if (const auto it = _callbacks.find(uint32_t(data)); it != _callbacks.end())
            it->second();
        else if (const auto it = _bool_callbacks.find(uint32_t(data)); it != _bool_callbacks.end())
            it->second(win32::checkbox(_menu, it->first));
        if (_command_callback) _command_callback(uint32_t(data));
        for (auto& m : _submenus) m.poll(msg, data);
    }
}

menu_bar& menu_bar::add_item(const menu& submenu)
{
    AppendMenu(_menu, MF_STRING | MF_POPUP, reinterpret_cast<UINT>(submenu._menu), submenu._name.c_str());
    SetMenu(_window, _menu);
    _submenus.push_back(submenu);
    return *this;
}

LRESULT CALLBACK window_callback(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        int   aElements[2]  = {COLOR_WINDOW, COLOR_ACTIVECAPTION};
        DWORD aNewColors[2] = {RGB(255, 255, 255), RGB(255, 255, 255)};
        SetSysColors(2, aElements, aNewColors);
        SetWindowLongPtr(window, (-21u), reinterpret_cast<LONG_PTR>(reinterpret_cast<LPCREATESTRUCT>(lparam)->lpCreateParams));
    }
    break;
        break;
    case WM_DESTROY: { exit(0);
    }
    break;
    }
    auto w = reinterpret_cast<win32::window*>(GetWindowLongPtr(window, (-21u)));
    if (w && w->main_menu) w->main_menu->poll(message, wparam);
    return DefWindowProcW(window, message, wparam, lparam);
}
}    // namespace win32
}    // namespace v1
}    // namespace gfx

int main()
{
    gfx::win32::window win;
    win.main_menu = std::make_shared<gfx::win32::menu_bar>(win);
    win.main_menu
        ->add_item(gfx::win32::menu("File")
                       .button("Open...", [] {})
                       .button("Save...", [] {})
                       .separator()
                       .button("Close", [] { exit(1); })
                       .button("Exit##file", [] { exit(0); }))
        .add_item(gfx::win32::menu("Edit")
                      .button("Copy", [] {})
                      .button("Paste", [] {})
                      .separator()
                      .popup(gfx::win32::menu("Tools")
                                 .checkbox("Show Console", false, [](bool b) { ShowWindow(GetConsoleWindow(), b); })
                                 .button("Exit##tools", [] {})
                                 .button("Advanced", [] {}))
                      .button("Properties", [] {}));

    while (true) { win.poll(); }
}