#include <gfx/window.hpp>
#include <gfx/log.hpp>

int main()
{
    gfx::window window(gfx::api::opengl, "Title", 1280, 720);
    window.set_icon(gfx::image_file("ui/logo.svg", 1.f));
    
    while(window.update())
    {
        log_i << window.delta_time();
    }

    return 0;
}
