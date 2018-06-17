#include <gfx/ecs/ecs.hpp>
#include <gfx/imgui.hpp>
#include <gfx/log.hpp>
#include <gfx/window.hpp>

#include <fstream>
#include <iostream>
#include <tuple>
#include <utility>

int main()
{
    gfx::clogd << "This is a test number: " << gfx::style::underlined << gfx::color_fg(166) << 19;
    gfx::clogh << "This is a test number: " << 8;
    gfx::clogi << "This is a test number: " << 8;
    gfx::clogw << "This is a test number: " << 8;
    gfx::cloge << "This is a test number: " << 8;

    auto       win = std::make_shared<gfx::window>(gfx::apis::opengl::name, "ImGuizmo", 1280, 720);
    gfx::imgui gui(win);

    while(win->update())
    {
        gui.new_frame();

        gui.render();
    }
}
