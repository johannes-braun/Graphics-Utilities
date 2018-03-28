#pragma once

#include "types.hpp"

namespace wf
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
        native_handle _handle;
    };

    std::vector<monitor> get_monitors() noexcept;
}