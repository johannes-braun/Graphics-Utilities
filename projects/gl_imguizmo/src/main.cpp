#include <gfx/window.hpp>
#include <gfx/imgui.hpp>

int main()
{
    auto win = std::make_shared<gfx::window>(gfx::apis::opengl::name, "ImGuizmo", 1280, 720);
    gfx::imgui gui(win);

    while(win->update())
    {
        gui.new_frame();

        gui.render();
    }
}