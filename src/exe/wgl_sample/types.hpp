#pragma once

#include "win32_keys.hpp"
#include <filesystem>

namespace winfw
{
    namespace files = std::experimental::filesystem;

    enum class cursor_mode
    {
        none = 0,
        captured,
        center_captured
    };

    struct point { long x, y; };
    struct rect { long left, top, right, bottom; };

    using drop_fun = void(const files::path& path);
    using resize_fun = void(int x, int y);
    using maximize_fun = void(bool maximized);
    using minimize_fun = void(bool minimized);
    using key_fun = void(int key, bool down);
    using char_fun = void(wchar_t c);
    using scroll_fun = void(float x, float y);
}