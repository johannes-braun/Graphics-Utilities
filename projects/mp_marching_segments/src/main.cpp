#include <gfx/window.hpp>
#include <gfx/imgui.hpp>
#include <gfx/camera.hpp>

int main()
{
    auto window = std::make_shared<gfx::window>(gfx::apis::opengl::name, "[Mesh Processing] Marching Segments", 1280, 720);
    gfx::imgui imgui(window);
    gfx::image_file img("brick.png", gfx::bits::b8, 4);
    gl::texture texture(GL_TEXTURE_2D, img.width, img.height, GL_RGBA8, 1);
    texture.assign(GL_RGBA, GL_UNSIGNED_BYTE, img.bytes());
    gl::sampler sampler;

    while (window->update())
    {
        imgui.new_frame();



        imgui.render();
    }

    return 0;
}