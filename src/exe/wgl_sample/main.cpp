
#include <cstdint>
#include <winfw/window.hpp>
#include <winfw/monitor.hpp>
#include <winfw/gl_context.hpp>
#include <mygl/gl.hpp>
#include <jpu/log.hpp>

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

    winfw::gl::context context(window, {
        { GL_CONTEXT_MAJOR_VERSION_ARB, 4 },
        { GL_CONTEXT_MINOR_VERSION_ARB, 6 },
        { GL_CONTEXT_FLAGS_ARB, GL_CONTEXT_DEBUG_BIT_ARB },
        { GL_CONTEXT_PROFILE_MASK_ARB, GL_CONTEXT_CORE_PROFILE_BIT_ARB },
    });
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