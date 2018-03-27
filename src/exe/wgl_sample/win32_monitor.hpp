#pragma once

#include "types.hpp"

namespace winfw::platform::win32
{
    class window;
    struct monitor
    {
        std::wstring name;
        rect rect;
        point millis;

    private:
        friend window;
        friend std::vector<monitor> get_monitors() noexcept;
        void* _handle;
    };

    std::vector<monitor> get_monitors() noexcept;
}