#pragma once

#include "keys.hpp"
#include <filesystem>

namespace wf
{
    namespace files = std::experimental::filesystem;

    enum class cursor_mode
    {
        none = 0,
        captured,
        center_captured
    };
    enum class native_handle : uint64_t { null = 0 };

    struct point { long x, y; };
    struct rect { long left, top, right, bottom; };

    using drop_fun = void(const files::path& path);
    using resize_fun = void(int x, int y);
    using maximize_fun = void(bool maximized);
    using minimize_fun = void(bool minimized);
    using key_fun = void(int key, bool down, bool alt);
    using char_fun = void(wchar_t c);
    using scroll_fun = void(float x, float y);
}