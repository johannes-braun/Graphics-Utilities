#pragma once

#include "win32_window.hpp"

namespace wgl
{
    class context
    {
    public:
        context(const win32::window& window);
        ~context();

        void make_current() const noexcept;
        void swap_buffers() const noexcept;
        void set_swap_interval(int i) const noexcept;

#ifndef WGL_PUBLIC_HANDLES
    private:
#endif
        int(*wglSwapIntervalEXT)(int) = nullptr;

        using handle = void*;
        handle _device_context;
        handle _context;
    };
}