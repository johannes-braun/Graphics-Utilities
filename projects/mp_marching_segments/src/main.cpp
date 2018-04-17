#include <gfx/window.hpp>
#include <gfx/imgui.hpp>
#include <gfx/camera.hpp>

int main()
{
    gl::shader::set_include_directories(std::vector<gfx::files::path>{ "../shd", SOURCE_DIRECTORY "/global/shd" });
    auto window = std::make_shared<gfx::window>(gfx::apis::opengl::name, "[Mesh Processing] Marching Segments", 1280, 720);
    gfx::imgui imgui(window);
    gfx::image_file img("brick.png", gfx::bits::b8, 4);
    gl::texture texture(GL_TEXTURE_2D, img.width, img.height, GL_RGBA8, 1);
    texture.assign(GL_RGBA, GL_UNSIGNED_BYTE, img.bytes());
    gl::sampler sampler;

    struct data
    {
        glm::mat4 vp;
        uint64_t tex;
    };
    gl::buffer<data> buf(1, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT);
    buf.map(GL_MAP_WRITE_BIT | GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT);

    gfx::camera camera;
    gfx::camera_controller ctrl(window);

    gl::pipeline pp;
    pp[GL_VERTEX_SHADER]    = std::make_shared<gl::shader>("seg.vert");
    pp[GL_GEOMETRY_SHADER]  = std::make_shared<gl::shader>("seg.geom");
    pp[GL_FRAGMENT_SHADER]  = std::make_shared<gl::shader>("seg.frag");

    while (window->update())
    {
        imgui.new_frame();
        ctrl.update(camera);
        buf[0].vp = camera.projection() * camera.view();
        buf[0].tex = sampler.sample(texture);
        pp.bind();
        pp.bind_uniform_buffer(0, buf);
        pp.draw(GL_POINTS, img.width*img.height);

        imgui.render();
    }

    return 0;
}