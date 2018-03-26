
#include <cstdint>
#include "win32_window.hpp"
#include "wgl_context.hpp"
#include <mygl/gl.hpp>
#include <jpu/log.hpp>

int main()
{
    win32::window window("My Window", 1280, 720);
    wgl::context context(window);
    context.make_current();
    context.set_swap_interval(0);
    mygl::load();

    
    window.set_clipboard(L"I am very okay.");

    while (window.update())
    {
        float col[]{ 0.3f, 0.55f, 1.f, 1.f };
        glClearNamedFramebufferfv(gl_framebuffer_t::zero, GL_COLOR, 0, col);
        
        if (win32::key_press(win32::mouse_left))
            log_i << "PRESS PEE";

        win32::point cpos = window.cursor_position();
        log_h << cpos.x << ", " << cpos.y;

        std::wcout << window.get_clipboard();

        context.swap_buffers();
    }

    return 0;
}