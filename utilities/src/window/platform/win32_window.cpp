#define VK_USE_PLATFORM_WIN32_KHR
#define WINFW_CALLBACKS public:
#include "../window.hpp"

#if defined(WIN32)
#include "../monitor.hpp"
#include <stdexcept>
#include <codecvt>
#include <Windows.h>
#include <iostream>

#ifndef WM_MOUSEHWHEEL
#define WM_MOUSEHWHEEL 0x020E
#endif

LRESULT CALLBACK window_callback(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

namespace wf
{
    HWND handle_cast(native_handle h)
    {
        return reinterpret_cast<HWND>(h);
    }

    native_handle handle_cast(HWND h)
    {
        return reinterpret_cast<native_handle&>(h);
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
        _handle = handle_cast(CreateWindowW(win_class.lpszClassName, title.data(), win_flags, pos_x, pos_y, r.right - r.left, r.bottom - r.top, nullptr, nullptr, instance, this));
        SetWindowLongPtr(handle_cast(_handle), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        DragAcceptFiles(handle_cast(_handle), TRUE);

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

        HMONITOR mon = reinterpret_cast<HMONITOR>(m._handle);
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
        SetWindowLongPtrW(handle_cast(_handle), GWL_STYLE, WS_VISIBLE | WS_POPUP);
        SetWindowPos(handle_cast(_handle), nullptr, 0, 0, dm.dmPelsWidth, dm.dmPelsHeight, SWP_FRAMECHANGED);
        set_cursor_mode(_cursor_mode);
    }

    void window::fullscreen_monitor(std::nullptr_t) noexcept
    {
        if (!_is_fullscreen)
            return;
        _is_fullscreen = false;

        RECT r{ 0, 0, _last_window_size.x, _last_window_size.y };
        AdjustWindowRect(&r, WS_VISIBLE | WS_OVERLAPPEDWINDOW, false);
        SetWindowLongPtr(handle_cast(_handle), GWL_STYLE, WS_VISIBLE | WS_OVERLAPPEDWINDOW);
        SetWindowPos(handle_cast(_handle), nullptr, _last_window_position.x + r.left, _last_window_position.y + r.top, r.right - r.left, r.bottom - r.top, SWP_FRAMECHANGED);

        set_cursor_mode(_cursor_mode);
    }

    monitor window::get_monitor() const noexcept
    {
        HMONITOR mon = MonitorFromWindow(handle_cast(_handle), MONITOR_DEFAULTTONEAREST);

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
        result._handle = reinterpret_cast<native_handle&>(mon);

        DeleteDC(dc);
        return result;
    }

    bool window::poll()
    {
        _last_update_time = _update_time;
        _update_time = std::chrono::system_clock::now();
        MSG msg{ 0 };
        if (PeekMessage(&msg, handle_cast(_handle), 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        _last_cursor_position = cursor_position();
        if (_cursor_mode == cursor_mode::center_captured)
        {
            rect b = get_rect();
            SetCursorPos((b.left + b.right) / 2, (b.top + b.bottom) / 2);
        }

        return !_closes;
    }

    rect window::get_rect() const noexcept
    {
        POINT offset{ 0 };
        ClientToScreen(handle_cast(_handle), &offset);
        rect rect{ 0, 0, 0, 0 };
        GetClientRect(handle_cast(_handle), reinterpret_cast<RECT*>(&rect));
        rect.left += offset.x;
        rect.right += offset.x;
        rect.top += offset.y;
        rect.bottom += offset.y;
        return rect;
    }

    point cursor_position() noexcept
    {
        point p;
        GetCursorPos(reinterpret_cast<LPPOINT>(&p));
        return p;
    }

    void set_cursor_position(point p) noexcept
    {
        SetCursorPos(p.x, p.y);
    }

    bool is_key_down(int key) noexcept
    {
        return GetAsyncKeyState(key) & (1 << 15);
    }

    window::~window()
    {
        if (_handle != native_handle::null)
            DestroyWindow(handle_cast(_handle));
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

    bool window::has_focus() const noexcept
    {
        return handle_cast(_handle) == GetFocus();
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

#if __has_include(<vulkan/vulkan.hpp>)
    vk::SurfaceKHR window::create_surface(vk::Instance instance, vk::Optional<const vk::AllocationCallbacks> callbacks) const noexcept
    {
        vk::Win32SurfaceCreateInfoKHR info({}, GetModuleHandleW(nullptr), handle_cast(_handle));
        return instance.createWin32SurfaceKHR(info, callbacks);
    }
#endif
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
    wf::window* parent = reinterpret_cast<wf::window*>(GetWindowLongPtr(window, GWLP_USERDATA));
    if (!parent)
        return DefWindowProcW(window, message, wparam, lparam);

    bool down = false;
    switch (message)
    {
    case WM_MOUSEWHEEL:
    {
        WORD keys = GET_KEYSTATE_WPARAM(wparam);
        short delta = GET_WHEEL_DELTA_WPARAM(wparam);
        parent->dispatch_scroll(delta / float(WHEEL_DELTA), 0.f);
    }
    break;
    case WM_MOUSEHWHEEL:
    {
        WORD keys = GET_KEYSTATE_WPARAM(wparam);
        short delta = GET_WHEEL_DELTA_WPARAM(wparam);
        parent->dispatch_scroll(0.f, delta / float(WHEEL_DELTA));
    }
    break;

    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
        parent->dispatch_key(int(wparam), !(message & 1), true);
        break;
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_XBUTTONDOWN:
        down = true;
    case WM_RBUTTONUP:
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_XBUTTONUP:
        switch (wparam)
        {
        case MK_LBUTTON:
            parent->dispatch_key(wf::mouse_left, down, false);
            break;
        case MK_MBUTTON:
            parent->dispatch_key(wf::mouse_middle, down, false);
            break;
        case MK_RBUTTON:
            parent->dispatch_key(wf::mouse_right, down, false);
            break;
        case MK_XBUTTON1:
            parent->dispatch_key(wf::mouse_x1, down, false);
            break;
        case MK_XBUTTON2:
            parent->dispatch_key(wf::mouse_x2, down, false);
            break;
        } break;
    case WM_KEYDOWN:
    case WM_KEYUP:
        parent->dispatch_key(int(wparam), !(message & 1), false);
        break;

    case WM_SYSCHAR:
    case WM_CHAR:
        parent->dispatch_char(wchar_t(wparam));
        break;

    case WM_SIZE:
    {
        parent->set_cursor_mode(parent->get_cursor_mode());
        // https://msdn.microsoft.com/en-us/library/windows/desktop/ms632646(v=vs.85).aspx
        int client_width = lparam & 0xffff;
        int client_height = (lparam & (0xffff << 16)) >> 16;
        int mode = int(wparam);

        if (mode == SIZE_RESTORED)
        {
            parent->dispatch_resize(client_width, client_height);
            parent->dispatch_maximize(false);
            parent->dispatch_minimize(false);
        }
        if (mode == SIZE_MAXIMIZED)
            parent->dispatch_maximize(true);
        if (mode == SIZE_MINIMIZED)
            parent->dispatch_minimize(true);
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
            parent->dispatch_drop(filename);
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
#endif // defined(Win32)