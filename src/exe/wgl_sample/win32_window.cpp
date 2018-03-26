#define WIN32_WINDOW_PUBLIC_HANDLES
#define WIN32_WINDOW_PUBLIC_CALLBACKS
#include "win32_window.hpp"
#include <stdexcept>
#include <codecvt>
#include <Windows.h>
#include <iostream>

LRESULT CALLBACK window_callback(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

namespace win32
{
    struct window::message
    {
        MSG msg{ 0 };
    };

    window::window(const std::string& title, int width, int height, int pos_x, int pos_y)
        : window(std::wstring(title.begin(), title.end()), width, height, pos_x, pos_y)
    {
        
    }

    window::window(const std::wstring& title, int width, int height, int pos_x, int pos_y)
    {
        const HINSTANCE instance = GetModuleHandle(nullptr);

        WNDCLASSW win_class{ 0 };
        win_class.lpfnWndProc = window_callback;
        win_class.hInstance = instance;
        win_class.hbrBackground = HBRUSH(COLOR_BACKGROUND);
        win_class.lpszClassName = L"WGL Sample";
        win_class.style = CS_OWNDC;
        win_class.hCursor = LoadCursor(0, IDC_ARROW);
        
        if (!RegisterClassW(&win_class))
            throw std::runtime_error("Failed to register Win32 window class.");

        bool resizable = true;
        bool enable_drag_drop = true;
        unsigned long win_flags = 
            WS_VISIBLE | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | 
            (resizable ? WS_THICKFRAME | WS_MAXIMIZEBOX : 0) |
            (enable_drag_drop ? WS_EX_ACCEPTFILES : 0);

        RECT r{ 0, 0, width, height };
        AdjustWindowRect(&r, win_flags, false);
        _handle = CreateWindowW(win_class.lpszClassName, L"WGL Sample", win_flags, pos_x, pos_y, r.right - r.left, r.bottom - r.top, nullptr, nullptr, instance, this);
        SetWindowLongPtr(static_cast<HWND>(_handle), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        DragAcceptFiles(static_cast<HWND>(_handle), TRUE);

        _msg = std::make_unique<message>();
        _creation_time = std::chrono::system_clock::now();
        _update_time = _creation_time;
    }

    bool window::update()
    {
        _last_update_time = _update_time;
        _update_time = std::chrono::system_clock::now();
        if (PeekMessage(&(_msg->msg), static_cast<HWND>(_handle), 0, 0, PM_REMOVE))
        {
            TranslateMessage(&(_msg->msg));
            DispatchMessage(&(_msg->msg));
        }

        return !_closes;
    }

    rect window::bounds() const noexcept
    {
        POINT offset{ 0 };
        ClientToScreen(static_cast<HWND>(_handle), &offset);
        rect rect{ 0, 0, 0, 0 };
        GetClientRect(static_cast<HWND>(_handle), reinterpret_cast<RECT*>(&rect));
        rect.left += offset.x;
        rect.right += offset.x;
        rect.top += offset.y;
        rect.bottom += offset.y;
        return rect;
    }

    point window::cursor_position() const noexcept
    {
        rect rect = bounds();
        point p = win32::cursor_position();
        p.x -= rect.left;
        p.y -= rect.top;
        return p;
    }

    point cursor_position() noexcept
    {
        point p;
        GetCursorPos(reinterpret_cast<LPPOINT>(&p));
        return p;
    }

    bool key_down(int key) noexcept
    {
        return GetAsyncKeyState(key) & (1 << 15);
    }

    bool key_press(int key) noexcept
    {
        return GetAsyncKeyState(key) & (1 << 0);
    }

    window::duration_type window::time_open() const noexcept
    {
        return std::chrono::system_clock::now() - _creation_time;
    }

    window::duration_type window::time_update() const noexcept
    {
        return _update_time - _last_update_time;
    }

    window::~window()
    {
        if(_handle)
            DestroyWindow(static_cast<HWND>(_handle));
    }

    void window::close()
    {
        _closes = true;
    }

    std::wstring window::get_clipboard() const
    {
        struct clipboard
        {
            clipboard() { OpenClipboard(nullptr); }
            ~clipboard() { CloseClipboard(); }
        } board;

        wchar_t* data = static_cast<wchar_t*>(GetClipboardData(CF_UNICODETEXT));
        return data ? data : L"";
    }

    void window::set_clipboard(const std::wstring& data) const
    {
        struct clipboard
        {
            clipboard() { OpenClipboard(nullptr); }
            ~clipboard() { CloseClipboard(); }
        } board;

        const size_t len = data.size();
        HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, len * sizeof(wchar_t));
        memcpy(GlobalLock(mem), data.data(), len * sizeof(wchar_t));
        GlobalUnlock(mem);

        EmptyClipboard();
        SetClipboardData(CF_UNICODETEXT, mem);
    }

    void window::on_drop(const files::path& path) const
    {

    }

    void window::on_resize(int x, int y) const 
    {

    }

    void window::on_maximize(bool maximized) const
    {

    }

    void window::on_minimize(bool minimized) const
    {

    }

    void window::on_key(int key, bool down, bool ctrl, bool alt, bool shift) const
    {

    }

    void window::on_char(wchar_t c, bool ctrl, bool alt, bool shift) const
    {

    }
}

LRESULT CALLBACK window_callback(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
    struct char_data
    {
        uint16_t rep_count : 15;
        uint16_t scan_code : 7;
        uint16_t extended : 1;
    private:
        uint16_t _reserved : 3;
    public:
        uint16_t alt_context : 1;
        uint16_t previous_state : 1;
        uint16_t transition_state : 1;
    };
    char_data& character_data = reinterpret_cast<char_data&>(lparam);
    win32::window* parent = reinterpret_cast<win32::window*>(GetWindowLongPtr(window, GWLP_USERDATA));
    switch (message)
    {
    case WM_SYSKEYDOWN:
        parent->on_key(int(wparam), false, GetKeyState(VK_CONTROL), true, GetKeyState(VK_SHIFT));
        break;
    case WM_KEYDOWN:
        parent->on_key(int(wparam), false, GetKeyState(VK_CONTROL), false, GetKeyState(VK_SHIFT));
        break;
    case WM_SYSKEYUP:
        parent->on_key(int(wparam), true, GetKeyState(VK_CONTROL), true, GetKeyState(VK_SHIFT));
        break;
    case WM_KEYUP:
        parent->on_key(int(wparam), true, GetKeyState(VK_CONTROL), false, GetKeyState(VK_SHIFT));
        break;

    case WM_SYSCHAR:
        parent->on_char(wchar_t(wparam), GetKeyState(VK_CONTROL), true, GetKeyState(VK_SHIFT));
        break;
    case WM_CHAR:
        parent->on_char(wchar_t(wparam), GetKeyState(VK_CONTROL), false, GetKeyState(VK_SHIFT));
        break;
    case WM_SIZE:
    {
        // https://msdn.microsoft.com/en-us/library/windows/desktop/ms632646(v=vs.85).aspx
        int client_width = lparam & 0xffff;
        int client_height = (lparam & (0xffff << 16)) >> 16;
        int mode = int(wparam);

        if (mode == SIZE_RESTORED)
        {
            parent->on_resize(client_width, client_height);
            parent->on_maximize(false);
            parent->on_minimize(false);
        }
        if (mode == SIZE_MAXIMIZED)
            parent->on_maximize(true);
        if (mode == SIZE_MINIMIZED)
            parent->on_minimize(true);
    }
    break;
    case WM_DROPFILES:
    {
        HDROP drop = reinterpret_cast<HDROP>(wparam);
        UINT count = DragQueryFileW(drop, ~0, nullptr, 0);
        for (UINT i = 0; i < count; ++i)
        {
            std::wstring filename(DragQueryFileW(drop, i, nullptr, 0), L' ');
            DragQueryFileW(drop, i, filename.data(), UINT(filename.size()+1));
            parent->on_drop(filename);
        }
        DragFinish(drop);
    }
    break;
    case WM_CLOSE: parent->close(); break;
    case WM_DESTROY: parent->close(); PostQuitMessage(0); break;
    default: break;
    }
    return DefWindowProcW(window, message, wparam, lparam);
}