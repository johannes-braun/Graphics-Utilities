#pragma comment( \
    linker,      \
    "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "gfx/gfx.hpp"
#include "gfx/math/geometry.hpp"
#include <CommCtrl.h>
#include <Shlwapi.h>
#include <Windows.h>
#include <cassert>
#include <functional>
#include <gfx/type.hpp>
#include <optional>
#include <string>
#include <utility>
#include <winuser.h>

namespace gfx {
inline namespace v1 {
namespace win32 {
class menu_bar;
class control;
class layout;

enum class style : uint8_t
{
    none      = 0,
    resizable = 1 << 0,
    min       = 1 << 1,
    max       = 1 << 2,
};
using style_flags = gfx::flags<uint8_t, style>;


class window
{
public:
    friend class menu_bar;
    friend class control;
    window(const std::wstring& title, style_flags style = style::none);

    void poll() const
    {
        MSG msg{};
        if (PeekMessage(&msg, _hnd, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    void add(layout& l);
	void add(control& c, layout& l, const glm::vec2& size, const glm::vec2& paddings ={ 0, 0 });

    friend LRESULT CALLBACK window_callback(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

    void position(const gfx::rect2f& rect) { position(rect.min, rect.size()); }
    void position(const glm::vec2& pos, const glm::vec2& size)
    {
        _content_rect = {pos, pos + size};

        MENUBARINFO mbi = {0};
        mbi.cbSize      = sizeof(mbi);
        GetMenuBarInfo(_hnd, OBJID_MENU, 0, &mbi);

        RECT rect;
        rect.left   = pos.x;
        rect.right  = pos.x + size.x;
        rect.top    = pos.y;
        rect.bottom = pos.y + size.y + (mbi.rcBar.bottom - mbi.rcBar.top);
        AdjustWindowRectEx(&rect, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE, 0);

        SetWindowPos(_hnd, nullptr, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, 0);
    }

    void set_menu(menu_bar& menu);
    void set_menu(std::nullptr_t);

private:
    menu_bar*                              _main_menu = nullptr;
    gfx::rect2f                            _content_rect;
    HWND                                   _hnd;
    std::unordered_map<uint32_t, control*> _controls;
};

LRESULT CALLBACK window_callback(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

window::window(const std::wstring& title, style_flags style) : _content_rect{{100, 100}, {400, 300}}
{
    static std::atomic_int id = 0;
    INITCOMMONCONTROLSEX   iccx;
    iccx.dwSize = sizeof(iccx);
    iccx.dwICC  = ICC_STANDARD_CLASSES;
    BOOL bRet   = ::InitCommonControlsEx(&iccx);
    assert(bRet);    // from crtcbg.h

    const HINSTANCE instance = GetModuleHandle(nullptr);

    WNDCLASSW wc{0};
    wc.lpfnWndProc   = window_callback;
    wc.hInstance     = instance;
    wc.hbrBackground = HBRUSH(COLOR_WINDOW);
    std::wstring cl  = L"gfx::win32::window" + std::to_wstring(id++);
    wc.lpszClassName = cl.c_str();
    wc.style         = CS_OWNDC;
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);

    if (!RegisterClassW(&wc)) throw "up";

    bool resizable   = (style & style::resizable) == style::resizable;
    bool minimizable = (style & style::min) == style::min;
    bool maximizable = (style & style::max) == style::max;

    const auto flags = WS_OVERLAPPED | WS_VISIBLE | WS_SYSMENU | (resizable * WS_THICKFRAME) | (minimizable * WS_MINIMIZEBOX)
                       | (maximizable * WS_MAXIMIZEBOX);
    ShowWindow(GetConsoleWindow(), false);
    _hnd = CreateWindowW(cl.c_str(), title.c_str(), flags, 50, 50, 1280, 720, nullptr, nullptr, instance, this);
    position(_content_rect);
}

bool call_checkbox(HMENU menu, UINT menu_item)
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
    friend class window;
    template<typename... T>
    using basic_callback = std::function<void(T...)>;

    using callback      = basic_callback<>;
    using cmd_callback  = basic_callback<u32>;
    using bool_callback = basic_callback<bool>;

    menu_bar() : _menu(CreateMenu()) {}
    menu_bar& add_item(const menu& submenu);

    void poll(uint32_t msg, uint64_t data);

    void set_callback(cmd_callback callback) { _command_callback = callback; }

private:
    std::vector<menu>                           _submenus;
    cmd_callback                                _command_callback;
    std::unordered_map<uint32_t, callback>      _callbacks;
    std::unordered_map<uint32_t, bool_callback> _bool_callbacks;
    HMENU                                       _menu;
};

void window::set_menu(menu_bar& menu)
{
    _main_menu = &menu;
    SetMenu(_hnd, menu._menu);
    position(_content_rect);
}

void window::set_menu(std::nullptr_t)
{
    _main_menu = nullptr;
    SetMenu(_hnd, nullptr);
    position(_content_rect);
}

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
                it->second(win32::call_checkbox(_menu, it->first));
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
            it->second(win32::call_checkbox(_menu, it->first));
        if (_command_callback) _command_callback(uint32_t(data));
        for (auto& m : _submenus) m.poll(msg, data);
    }
}

menu_bar& menu_bar::add_item(const menu& submenu)
{
    AppendMenu(_menu, MF_STRING | MF_POPUP, reinterpret_cast<UINT>(submenu._menu), submenu._name.c_str());
    // SetMenu(_window, _menu);
    _submenus.push_back(submenu);
    return *this;
}

class control
{
public:
    friend class window;

    control() : _id(nextID()) {}
    virtual ~control()
    {
        if (_widget) DestroyWindow(_widget);
    }

    virtual std::wstring_view label() { return L""; }
    virtual void              on_command(uint64_t w, int64_t l) = 0;
    virtual std::wstring_view class_name()                      = 0;
    virtual uint32_t          style()                           = 0;

protected:
    HWND     _parent = nullptr;
    uint32_t _id;
    HWND     _widget = nullptr;

private:
    static uint32_t nextID()
    {
        static uint32_t i = 0;
        return i++;
    }
};

class layout : public control
{
public:
    virtual ~layout()                                         = default;
    virtual gfx::rect2f       position(const glm::vec2& size) = 0;
    void                      on_command(uint64_t w, int64_t l) override { std::cout << "abldlksbndasddasd"; }
    virtual std::wstring_view label() { return L""; }
    std::wstring_view         class_name() override
    {
        const HINSTANCE instance = GetModuleHandle(nullptr);

        WNDCLASSW wc{0};
        wc.lpfnWndProc   = window_callback;
        wc.hInstance     = instance;
        wc.hbrBackground = HBRUSH(COLOR_WINDOW);
        _cls             = L"gfx::win32::layout" + std::to_wstring(_id);
        wc.lpszClassName = _cls.c_str();
        wc.style         = CS_OWNDC;
        wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);

        if (!RegisterClassW(&wc)) throw "up";
        return _cls.c_str();
    }
    uint32_t style() override { return WS_CLIPCHILDREN; }

private:
    std::wstring _cls;
};

class linear_layout : public layout
{
public:
    enum class orient
    {
        horizontal,
        vertical
    };

    linear_layout(orient o = orient::vertical) : _orient(o) {}

    gfx::rect2f position(const glm::vec2& size) override
    {
        const auto  pos = _orient == orient::vertical ? glm::vec2{0, _ptr} : glm::vec2{_ptr, 0};
        gfx::rect2f r(pos, pos + size);
        _ptr += size[_orient == orient::vertical];
        return r;
    }

private:
    orient _orient;
    float  _ptr = 0;
};

void window::add(layout& l)
{
    l._id = control::nextID();
    _controls.emplace(l._id, &l);
    const auto r    = _content_rect;
    const auto size = r.size();

    l._parent = _hnd;
    if (l._widget) DestroyWindow(l._widget);
    l._widget         = CreateWindowW(l.class_name().data(),                             // Predefined class; Unicode assumed
                              l.label().data(),                                  // Button text
                              WS_TABSTOP | WS_VISIBLE | WS_CHILD | l.style(),    // Styles
                              0,                                                 // x position
                              0,                                                 // y position
                              size.x,                                            // Button width
                              size.y,                                            // Button height
                              _hnd,                                              // Parent window
                              HMENU(l._id),                                      // No menu.
                              (HINSTANCE)GetWindowLong(_hnd, (-6)),
                              this);    // Pointer not needed.
    HFONT defaultFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    SendMessage(l._widget, WM_SETFONT, WPARAM(defaultFont), TRUE);
}

void window::add(control& c, layout& l, const glm::vec2& s, const glm::vec2& paddings)
{
    c._id = control::nextID();
    _controls.emplace(c._id, &c);
    auto r    = l.position(s);
	r.inset(paddings, paddings);
    const auto size = r.size();

    if (c._widget) DestroyWindow(c._widget);
    c._widget         = CreateWindowW(c.class_name().data(),                             // Predefined class; Unicode assumed
                              c.label().data(),                                  // Button text
                              WS_TABSTOP | WS_VISIBLE | WS_CHILD | c.style(),    // Styles
                              r.min.x,                                           // x position
                              r.min.y,                                           // y position
                              size.x,                                            // Button width
                              size.y,                                            // Button height
                              l._widget,                                         // Parent window
                              HMENU(c._id),                                      // No menu.
                              (HINSTANCE)GetWindowLong(l._widget, (-6)),
                              this);    // Pointer not needed.
    HFONT defaultFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    SendMessage(c._widget, WM_SETFONT, WPARAM(defaultFont), TRUE);
    c._parent = l._widget;
}

class checkbox : public control
{
public:
    checkbox(const std::wstring& label, std::function<void(bool)> oncheck) : _label(label), _oncheck(std::move(oncheck)) {}
    std::wstring_view label() override { return _label; }
    std::wstring_view class_name() override { return L"Button"; }
    uint32_t          style() override { return BTNS_CHECK; }
    void              on_command(uint64_t w, int64_t l) override
    {
        if (IsDlgButtonChecked(_parent, _id))
        {
            CheckDlgButton(_parent, _id, BST_UNCHECKED);
            _oncheck(false);
        }
        else
        {
            CheckDlgButton(_parent, _id, BST_CHECKED);
            _oncheck(true);
        }
    }

private:
    std::wstring              _label;
    std::function<void(bool)> _oncheck;
};

class button : public control
{
public:
    button(const std::wstring& label, std::function<void()> onclick) : _label(label), _onclick(std::move(onclick)) {}
    void on_command(uint64_t w, int64_t l) override
    {
        if (_onclick) _onclick();
    }
    virtual std::wstring_view label() { return _label; }
    std::wstring_view         class_name() override { return L"Button"; }
    uint32_t                  style() override { return BTNS_BUTTON; }

private:
    std::wstring          _label;
    std::function<void()> _onclick;
};

class edittext : public control
{
public:
    edittext(std::optional<std::function<void(std::wstring_view)>> onchanged = std::nullopt)
    {
        if (onchanged) _onchanged = *onchanged;
    }

    std::wstring_view class_name() override { return L"EDIT"; }
    uint32_t          style() override { return WS_BORDER | WS_EX_STATICEDGE; }
    void              on_command(uint64_t w, int64_t l) override
    {
        const int len = GetWindowTextLengthW(_widget);
        _content.resize(len);
        GetWindowTextW(_widget, _content.data(), len);
        if (_onchanged) _onchanged(_content);
    }

private:
    std::wstring                           _content;
    std::function<void(std::wstring_view)> _onchanged;
};

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

        HFONT hFont = CreateFont(13, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
                                 DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Tahoma"));
        SendMessage(window, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
    }
    break;
    case WM_DESTROY: { exit(0);
    }
    break;
    }
    auto w = reinterpret_cast<win32::window*>(GetWindowLongPtr(window, (-21u)));
    if (w && w->_main_menu) w->_main_menu->poll(message, wparam);
    if (w)
    {
        for (auto& c : w->_controls)
        {
            if (message == WM_COMMAND)
                if (LOWORD(wparam) == c.first) c.second->on_command(wparam, lparam);
        }
    }
    return DefWindowProcW(window, message, wparam, lparam);
}
}    // namespace win32
namespace ui = win32;
}    // namespace v1
}    // namespace gfx

int main()
{
	const glm::vec2 pad{ 4, 4 };
    gfx::ui::window   win(L"Yet another window");
    gfx::ui::menu_bar main_menu;
    main_menu
        .add_item(gfx::ui::menu("File")
                      .button("Open...", [] {})
                      .button("Save...", [] {})
                      .separator()
                      .button("Close", [] { exit(1); })
                      .button("Exit##file", [] { exit(0); }))
        .add_item(gfx::ui::menu("Edit")
                      .button("Copy", [] {})
                      .button("Paste", [] {})
                      .separator()
                      .popup(gfx::ui::menu("Tools")
                                 .checkbox("Show Console", false, [](bool b) { ShowWindow(GetConsoleWindow(), b); })
                                 .button("Exit##tools", [] {})
                                 .button("Advanced", [] {}))
                      .button("Properties", [] {}));
    win.position({100, 100}, glm::vec2{288, 126} + 2*pad);
    win.set_menu(main_menu);

    gfx::ui::button   btn1(L"Click me", []() { gfx::ilog << "One"; });
    gfx::ui::button   btn2(L"Click me too", []() { gfx::ilog << "Two"; });
    gfx::ui::checkbox check(L"Check me out", [&](bool c) {
        gfx::ilog << "Checked: " << c;
        if (c)
            win.set_menu(main_menu);
        else
            win.set_menu(nullptr);
    });
    gfx::ui::edittext edit([](std::wstring_view s) { std::wcout << s.data(); });
    gfx::ui::button   dotted(L"...", []() { gfx::ilog << "Dotdotdot"; });

    gfx::ui::linear_layout ll(gfx::ui::linear_layout::orient::vertical);
    gfx::ui::linear_layout btns(gfx::ui::linear_layout::orient::horizontal);
    gfx::ui::linear_layout tb(gfx::ui::linear_layout::orient::horizontal);
    win.add(ll);
    win.add(btns, ll, glm::vec2{288, 48} + 2.f*pad, pad);
    win.add(btn1, btns, {144, 48}, pad);
    win.add(btn2, btns, {144, 48}, pad);
    win.add(tb, ll, glm::vec2{288, 28} + 2.f*pad, pad);
    win.add(edit, tb, {256, 28}, pad);
    win.add(dotted, tb, {32, 28}, pad);
    win.add(check, ll, {288, 32}, pad);

    gfx::ui::window win2(L"Also there");

    win.set_menu(nullptr);
    win2.set_menu(main_menu);

    while (true)
    {
        win.poll();
        win2.poll();
    }
}