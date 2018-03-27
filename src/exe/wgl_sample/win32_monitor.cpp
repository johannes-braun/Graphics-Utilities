#define WINFW_HANDLES public:

#include "win32_monitor.hpp"
#include "win32_window.hpp"
#include <Windows.h>

namespace winfw::platform::win32
{
    std::vector<monitor> get_monitors() noexcept
    {
        std::vector<monitor> result;
        for (int ai = 0; ; ++ai)
        {
            DISPLAY_DEVICEW device{ 0 };
            device.cb = sizeof(DISPLAY_DEVICEW);
            if (!EnumDisplayDevicesW(nullptr, ai, &device, 0))
                break;
            if (!(device.StateFlags & DISPLAY_DEVICE_ACTIVE))
                continue;

            DEVMODEW mode{ 0 };
            mode.dmSize = sizeof(DEVMODEW);
            EnumDisplaySettingsW(device.DeviceName, ENUM_CURRENT_SETTINGS, &mode);
            HDC dc = CreateDCW(L"display", device.DeviceName, nullptr, nullptr);

            monitor& mon = result.emplace_back();
            mon.name = device.DeviceName;
            mon.rect.left = mode.dmPosition.x;
            mon.rect.top = mode.dmPosition.y;
            mon.rect.right = mode.dmPosition.x + mode.dmPelsWidth;
            mon.rect.bottom = mode.dmPosition.y + mode.dmPelsHeight;
            mon.millis.x = GetDeviceCaps(dc, HORZSIZE);
            mon.millis.y = GetDeviceCaps(dc, VERTSIZE);

            DeleteDC(dc);

            EnumDisplayMonitors(nullptr, reinterpret_cast<LPRECT>(&mon.rect), [](HMONITOR handle, HDC dc, RECT* rect, LPARAM data) -> BOOL {
                MONITORINFOEXW info{};
                ZeroMemory(&info, sizeof(info));
                info.cbSize = sizeof(MONITORINFOEXW);

                if (GetMonitorInfoW(handle, &info) && reinterpret_cast<monitor*>(data)->name == info.szDevice)
                    reinterpret_cast<monitor*>(data)->_handle = handle;

                return true;
            }, reinterpret_cast<LPARAM>(&mon));
        }
        return result;
    }
}