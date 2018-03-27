#define WINFW_HANDLES public:
#define WINFW_CALLBACKS public:
#include "win32_window.hpp"
#include "win32_monitor.hpp"
#include <stdexcept>
#include <codecvt>
#include <Windows.h>
#include <iostream>

LRESULT CALLBACK window_callback(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

namespace winfw::platform::win32
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
        win_class.lpszClassName = title.data();
        win_class.style = CS_OWNDC;
        win_class.hCursor = LoadCursor(0, IDC_ARROW);

        if (!RegisterClassW(&win_class))
            throw std::runtime_error("Failed to register Win32 window class.");

        bool resizable = true;
        bool enable_drag_drop = true;
        unsigned long win_flags =
            WS_OVERLAPPED | WS_VISIBLE | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX |
            (resizable ? WS_THICKFRAME | WS_MAXIMIZEBOX : 0) |
            (enable_drag_drop ? WS_EX_ACCEPTFILES : 0);

        RECT r{ 0, 0, width, height };
        AdjustWindowRect(&r, win_flags, false);
        _handle = CreateWindowW(win_class.lpszClassName, title.data(), win_flags, pos_x, pos_y, r.right - r.left, r.bottom - r.top, nullptr, nullptr, instance, this);
        SetWindowLongPtr(static_cast<HWND>(_handle), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        DragAcceptFiles(static_cast<HWND>(_handle), TRUE);

        _msg = std::make_unique<message>();
        _creation_time = std::chrono::system_clock::now();
        _update_time = _creation_time;
    }

    void window::fullscreen_monitor(const monitor& m) noexcept
    {
        if (_is_fullscreen)
            return;

        _is_fullscreen = true;
        rect b = get_rect();
        _last_window_size = point{ b.right - b.left, b.bottom - b.top };
        _last_window_position = point{ b.left, b.top };

        HMONITOR mon = static_cast<HMONITOR>(m._handle);
        MONITORINFOEXW info{};
        ZeroMemory(&info, sizeof(info));
        info.cbSize = sizeof(MONITORINFOEXW);
        GetMonitorInfoW(mon, &info);

        DEVMODEW mode{ 0 };
        mode.dmSize = sizeof(DEVMODEW);
        EnumDisplaySettingsW(info.szDevice, ENUM_CURRENT_SETTINGS, &mode);

        DEVMODEW dm;
        ZeroMemory(&dm, sizeof(dm));
        dm.dmSize = sizeof(dm);
        dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY;
        dm.dmPelsWidth = mode.dmPelsWidth;
        dm.dmPelsHeight = mode.dmPelsHeight;
        dm.dmBitsPerPel = mode.dmBitsPerPel;
        dm.dmDisplayFrequency = mode.dmDisplayFrequency;

        auto res = ChangeDisplaySettingsExW(info.szDevice, &dm, nullptr, CDS_FULLSCREEN, nullptr);
        SetWindowLongPtrW(static_cast<HWND>(_handle), GWL_STYLE, WS_VISIBLE | WS_POPUP);
        SetWindowPos(static_cast<HWND>(_handle), nullptr, 0, 0, dm.dmPelsWidth, dm.dmPelsHeight, SWP_FRAMECHANGED);
    }

    bool window::is_fullscreen() const noexcept
    {
        return _is_fullscreen;
    }

    void window::fullscreen_monitor(std::nullptr_t) noexcept
    {
        if (!_is_fullscreen)
            return;
        _is_fullscreen = false;

        RECT r{ 0, 0, _last_window_size.x, _last_window_size.y };
        AdjustWindowRect(&r, WS_VISIBLE | WS_OVERLAPPEDWINDOW, false);
        SetWindowLongPtr(static_cast<HWND>(_handle), GWL_STYLE, WS_VISIBLE | WS_OVERLAPPEDWINDOW);
        SetWindowPos(static_cast<HWND>(_handle), nullptr, _last_window_position.x + r.left, _last_window_position.y + r.top, r.right - r.left, r.bottom - r.top, SWP_FRAMECHANGED);
    }

    monitor window::get_monitor() const noexcept
    {
        HMONITOR mon = MonitorFromWindow(static_cast<HWND>(_handle), MONITOR_DEFAULTTONEAREST);

        MONITORINFOEXW info{};
        ZeroMemory(&info, sizeof(info));
        info.cbSize = sizeof(MONITORINFOEXW);
        GetMonitorInfoW(mon, &info);

        DEVMODEW mode{ 0 };
        mode.dmSize = sizeof(DEVMODEW);
        EnumDisplaySettingsW(info.szDevice, ENUM_CURRENT_SETTINGS, &mode);
        HDC dc = CreateDCW(L"display", info.szDevice, nullptr, nullptr);

        monitor result;
        result.name = info.szDevice;
        result.rect.left = mode.dmPosition.x;
        result.rect.top = mode.dmPosition.y;
        result.rect.right = mode.dmPosition.x + mode.dmPelsWidth;
        result.rect.bottom = mode.dmPosition.y + mode.dmPelsHeight;
        result.millis.x = GetDeviceCaps(dc, HORZSIZE);
        result.millis.y = GetDeviceCaps(dc, VERTSIZE);
        result._handle = mon;

        DeleteDC(dc);
        return result;
    }

    bool window::poll()
    {
        _last_update_time = _update_time;
        _update_time = std::chrono::system_clock::now();
        if (PeekMessage(&(_msg->msg), static_cast<HWND>(_handle), 0, 0, PM_REMOVE))
        {
            TranslateMessage(&(_msg->msg));
            DispatchMessage(&(_msg->msg));
        }

        _last_cursor_position = cursor_position();
        if (_cursor_mode == cursor_mode::center_captured)
        {
            rect b = get_rect();
            SetCursorPos((b.left + b.right) / 2, (b.top + b.bottom) / 2);
        }

        return !_closes;
    }

    point window::get_cursor_delta() const noexcept
    {
        point p = cursor_position();
        return point{ p.x - _last_cursor_position.x, p.y - _last_cursor_position.y };
    }

    rect window::get_rect() const noexcept
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
        rect rect = get_rect();
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

    bool is_key_down(int key) noexcept
    {
        return GetAsyncKeyState(key) & (1 << 15);
    }

    bool is_key_press(int key) noexcept
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
        if (_handle)
            DestroyWindow(static_cast<HWND>(_handle));
    }

    void window::close() noexcept
    {
        _closes = true;
    }

    std::wstring get_clipboard_text() noexcept
    {
        struct clipboard {
            clipboard() { OpenClipboard(nullptr); }
            ~clipboard() { CloseClipboard(); }
        } board;

        wchar_t* data = static_cast<wchar_t*>(GetClipboardData(CF_UNICODETEXT));
        return data ? data : L"";
    }

    void set_clipboard_text(const std::wstring& data) noexcept
    {
        struct clipboard {
            clipboard() { OpenClipboard(nullptr); }
            ~clipboard() { CloseClipboard(); }
        } board;

        const size_t len = data.size()+1;
        HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, len * sizeof(wchar_t));
        memcpy(GlobalLock(mem), data.data(), len * sizeof(wchar_t));
        GlobalUnlock(mem);

        EmptyClipboard();
        SetClipboardData(CF_UNICODETEXT, mem);
    }

    void window::set_cursor_mode(cursor_mode mode) noexcept
    {
        _cursor_mode = mode;

        rect r = get_rect();
        switch (mode)
        {
        case cursor_mode::none:
            ClipCursor(nullptr);
            ShowCursor(true);
            break;
        case cursor_mode::captured:
            ClipCursor(reinterpret_cast<LPRECT>(&r));
            ShowCursor(true);
            break;
        case cursor_mode::center_captured:
            ClipCursor(reinterpret_cast<LPRECT>(&r));
            ShowCursor(false);
            break;
        default:
            break;
        }
    }

    cursor_mode window::get_cursor_mode() const noexcept
    {
        return _cursor_mode;
    }

    void window::on_drop(const files::path& path) const
    {
        for (const auto& callback : drop_callbacks)
            callback(path);
    }

    void window::on_resize(int x, int y) const
    {
        for (const auto& callback : resize_callbacks)
            callback(x, y);
    }

    void window::on_maximize(bool maximized) const
    {
        for (const auto& callback : maximize_callbacks)
            callback(maximized);
    }

    void window::on_minimize(bool minimized) const
    {
        for (const auto& callback : minimize_callbacks)
            callback(minimized);
    }

    void window::on_key(int key, bool down) const
    {
        for (const auto& callback : key_callbacks)
            callback(key, down);
    }

    void window::on_char(wchar_t c) const
    {
        for (const auto& callback : char_callbacks)
            callback(c);
    }

    void window::on_scroll(float x, float y) const
    {
        for (const auto& callback : scroll_callbacks)
            callback(x, y);
    }
}

#ifndef WM_MOUSEHWHEEL
#define WM_MOUSEHWHEEL 0x020E
#endif

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
    winfw::platform::win32::window* parent = reinterpret_cast<winfw::platform::win32::window*>(GetWindowLongPtr(window, GWLP_USERDATA));
    if (!parent)
        return DefWindowProcW(window, message, wparam, lparam);

    switch (message)
    {
    case WM_MOUSEWHEEL:
    {
        WORD keys = GET_KEYSTATE_WPARAM(wparam);
        short delta = GET_WHEEL_DELTA_WPARAM(wparam);
        parent->on_scroll(delta / float(WHEEL_DELTA), 0.f);
    }
    break;
    case WM_MOUSEHWHEEL:
    {
        WORD keys = GET_KEYSTATE_WPARAM(wparam);
        short delta = GET_WHEEL_DELTA_WPARAM(wparam);
        parent->on_scroll(0.f, delta / float(WHEEL_DELTA));
    }
    break;

    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
    case WM_SYSKEYUP:
    case WM_KEYUP:
        parent->on_key(int(wparam), !(message & 1));
        break;

    case WM_SYSCHAR:
    case WM_CHAR:
        parent->on_char(wchar_t(wparam));
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
            DragQueryFileW(drop, i, filename.data(), UINT(filename.size() + 1));
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