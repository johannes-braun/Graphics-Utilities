#pragma once

#include "wgl_internal.hpp"
#include "win32_window.hpp"

namespace wgl
{
    class context
    {
        using handle = void*;
    public:
        context(const winfw::platform::win32::window& window);
        ~context();

        void make_current() const noexcept;
        void swap_buffers() const noexcept;
        void set_swap_interval(int i) const noexcept;

    WGL_HANDLES
        int(*wglSwapIntervalEXT)(int) = nullptr;
        handle _device_context;
        handle _context;
    };
}