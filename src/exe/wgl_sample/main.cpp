
#include <cstdint>
#include "win32_window.hpp"
#include "win32_monitor.hpp"
#include "wgl_context.hpp"
#include <mygl/gl.hpp>
#include <jpu/log.hpp>

namespace winfw
{
    using namespace platform::win32;
}

int main()
{
    winfw::window window(L"My Window", 1280, 720);
    window.key_callbacks.emplace_back([](int k, bool down) {
        if (down && k == 'V' && winfw::is_key_down(winfw::key_control))
            std::wcout << winfw::get_clipboard_text();
    });
    
    window.resize_callbacks.emplace_back([](int x, int y) {
        log_i << "Resized to " << x << ", " << y;
    });

    wgl::context context(window);
    context.make_current();
    context.set_swap_interval(0);
    mygl::load();

    winfw::set_clipboard_text(L"Autschi");

    while (window.poll())
    {
        float col[]{ 0.3f, 0.55f, 1.f, 1.f };
        glClearNamedFramebufferfv(gl_framebuffer_t::zero, GL_COLOR, 0, col);
        
        if (winfw::is_key_press(winfw::mouse_right))
            window.set_cursor_mode(window.get_cursor_mode() == winfw::cursor_mode::captured ? winfw::cursor_mode::none : winfw::cursor_mode::captured);

        if (winfw::is_key_press(winfw::mouse_middle))
            window.is_fullscreen() ? window.fullscreen_monitor(nullptr) : window.fullscreen_monitor(window.get_monitor());

        context.swap_buffers();
    }

    return 0;
}