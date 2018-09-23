#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <Windows.h>
#include <functional>
#include <gfx/type.hpp>
#include <string>
#include <utility>
#include <winuser.h>
#include <CommCtrl.h>
#include <cassert>
#include <Shlwapi.h>
#include <optional>
#include "gfx/log.hpp"
#include "gfx/math/geometry.hpp"

namespace gfx {
inline namespace v1 {
namespace win32 {
class menu_bar;
class control;
class window
{
public:
    friend class menu_bar;
	friend class control;
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

	void add(control& c);

	friend LRESULT CALLBACK window_callback(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

private:
    HWND _hnd;
	std::unordered_map<uint32_t, control*> _controls;
};

LRESULT CALLBACK window_callback(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

window::window()
{
	INITCOMMONCONTROLSEX iccx;
	iccx.dwSize = sizeof(iccx);
	iccx.dwICC = ICC_STANDARD_CLASSES;
	BOOL bRet = ::InitCommonControlsEx(&iccx);
	assert(bRet); // from crtcbg.h 

    const HINSTANCE instance = GetModuleHandle(nullptr);

    WNDCLASSW wc{0};
    wc.lpfnWndProc   = window_callback;
    wc.hInstance     = instance;
	wc.hbrBackground = HBRUSH(COLOR_WINDOW );
    wc.lpszClassName = L"gfx::win32::window";
    wc.style         = CS_OWNDC;
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);

    if (!RegisterClassW(&wc)) throw "up";

    const auto flags = WS_OVERLAPPED | WS_VISIBLE | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME | WS_MAXIMIZEBOX;
    ShowWindow(GetConsoleWindow(), false);
    _hnd = CreateWindowW(L"gfx::win32::window", L"My Window", flags, 50, 50, 1280, 720, nullptr, nullptr, instance, this);
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
    SetMenu(_window, _menu);
    _submenus.push_back(submenu);
    return *this;
}

class control
{
public:
	friend class window;

	control() : _id(nextID()) {}
	virtual ~control() = default;

	virtual void on_command(uint64_t w, int64_t l) = 0;

protected:
	HWND win(window& w) { return w._hnd; }
	uint32_t _id;
private:
	static uint32_t nextID() { static uint32_t i = 0; return i++; }
};

void window::add(control& c)
{
	_controls.emplace(c._id, &c);
}

class checkbox : public control
{
public:
	checkbox(window& w, const std::wstring& label, const rect2f& rect, std::function<void(bool)> oncheck)
		: _oncheck(std::move(oncheck))
	{
		_parent = win(w);
		const auto size = rect.size();
		_btn = CreateWindowW( 
			L"BUTTON",  // Predefined class; Unicode assumed 
			label.c_str(),      // Button text 
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BTNS_CHECK,  // Styles 
			rect.min.x,         // x position 
			rect.min.y,         // y position 
			size.x,        // Button width
			size.y,        // Button height
			win(w),     // Parent window
			HMENU(_id),       // No menu.
			(HINSTANCE)GetWindowLong(win(w), (-6)), 
			NULL);      // Pointer not needed.
		HFONT defaultFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		SendMessage(_btn, WM_SETFONT, WPARAM (defaultFont), TRUE);
	}
	~checkbox()
	{
		DestroyWindow(_btn);
	}
	void on_command(uint64_t w, int64_t l) override
	{
		if (IsDlgButtonChecked(_parent, _id)) {
			CheckDlgButton(_parent, _id, BST_UNCHECKED);
			_oncheck(false);
		} else {
			CheckDlgButton(_parent, _id, BST_CHECKED);
			_oncheck(true);
		}
	}

private:
	HWND _parent = nullptr;
	HWND _btn = nullptr;
	std::function<void(bool)> _oncheck;
};

class button : public control
{
public:
    button(window& w, const std::wstring& label, const rect2f& rect, std::function<void()> onclick)
		: _onclick(std::move(onclick))
    {
		const auto size = rect.size();
		_btn = CreateWindowW( 
			L"BUTTON",  // Predefined class; Unicode assumed 
			label.c_str(),      // Button text 
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BTNS_BUTTON,  // Styles 
			rect.min.x,         // x position 
			rect.min.y,         // y position 
			size.x,        // Button width
			size.y,        // Button height
			win(w),     // Parent window
			HMENU(_id),       // No menu.
			(HINSTANCE)GetWindowLong(win(w), (-6)), 
			NULL);      // Pointer not needed.
		HFONT defaultFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		SendMessage(_btn, WM_SETFONT, WPARAM (defaultFont), TRUE);
    }
    ~button()
    {
		DestroyWindow(_btn);
    }
	void on_command(uint64_t w, int64_t l) override
    {
        if(_onclick)
		    _onclick();
    }

private:
	HWND _btn = nullptr;
	std::function<void()> _onclick;
};

class edittext : public control
{
public:
	edittext(window& w, const rect2f& rect, std::optional<std::function<void(std::wstring_view)>> onchanged = std::nullopt)
	{
		if (onchanged)
			_onchanged = *onchanged;
		const auto size = rect.size();
		_box = CreateWindowW( 
			L"EDIT",  // Predefined class; Unicode assumed 
			L"",      // Button text 
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER | WS_EX_STATICEDGE,  // Styles 
			rect.min.x,         // x position 
			rect.min.y,         // y position 
			size.x,        // Button width
			size.y,        // Button height
			win(w),     // Parent window
			HMENU(_id),       // No menu.
			(HINSTANCE)GetWindowLong(win(w), (-6)), 
			NULL);      // Pointer not needed.
		HFONT defaultFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		SendMessage(_box, WM_SETFONT, WPARAM (defaultFont), TRUE);
	}
    ~edittext()
	{
		DestroyWindow(_box);
	}

    void on_command(uint64_t w, int64_t l) override
	{
		const int len = GetWindowTextLengthW(_box);
		_content.resize(len);
		GetWindowTextW(_box, _content.data(), len);
		if (_onchanged)
			_onchanged(_content);
	}

private:
	HWND _box = nullptr;
	std::wstring _content;
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

		HFONT hFont = CreateFont (13, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, 
			OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
			DEFAULT_PITCH | FF_DONTCARE, TEXT("Tahoma"));
		SendMessage(window, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
    }
    break;
    case WM_DESTROY: { exit(0);
    }
    break;
    }
    auto w = reinterpret_cast<win32::window*>(GetWindowLongPtr(window, (-21u)));
    if (w && w->main_menu) w->main_menu->poll(message, wparam);
    if(w)
    {
        for(auto& c : w->_controls)
        {
			if (message == WM_COMMAND && LOWORD(wparam) == c.first)
				c.second->on_command(wparam, lparam);
        }
    }
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

	gfx::win32::button btn1(win, L"Click me", { {10, 10}, {140, 58} }, []() { gfx::ilog << "One"; });
	gfx::win32::button btn2(win, L"Click me too", {{148, 10}, {278, 58}}, []() { gfx::ilog << "Two"; });
	gfx::win32::checkbox check(win, L"Check me out", { {10, 64}, {278, 90} }, [](bool c) { gfx::ilog << "Checked: " << c; });
	gfx::win32::edittext edit(win, { {10, 96}, {248, 116} }, [](std::wstring_view s) { std::wcout << s.data(); });
	gfx::win32::button dotted(win, L"...", { {254, 96}, {278, 116} }, []() { gfx::ilog << "Dotdotdot"; });

	win.add(btn1);
	win.add(btn2);
	win.add(check);
	win.add(edit);
	win.add(dotted);

    while (true) { win.poll(); }
}