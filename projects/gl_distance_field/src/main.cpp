#include "gfx/window.hpp"
#include "stb_image.h"
#include "gfx/renderer.hpp"
#include "gfx/gizmo.hpp"
#include "gfx/file.hpp"
#include "gfx/imgui.hpp"
#include "gfx/camera.hpp"

std::unique_ptr<gfx::renderer> main_renderer;
std::shared_ptr<gfx::window> main_window;
std::unique_ptr<gl::pipeline> mypeline;

int main(int argc, const char** argv)
{
    gl::shader::set_include_directories(std::vector<gfx::files::path>{ "../shd", SOURCE_DIRECTORY "/global/shd" });

    gfx::window_hints hints;
    hints[GLFW_SAMPLES]                 = 4;
    hints[GLFW_OPENGL_DEBUG_CONTEXT]    = true;
    main_window = std::make_shared<gfx::window>(gfx::apis::opengl::name, "My Window", 1280, 720, hints);
    main_window->set_icon(gfx::image_file("ui/logo.png", gfx::bits::b8, 4));
    main_window->set_max_framerate(60.f);
    main_window->key_callback.add([](GLFWwindow*, int key, int, int action, int mods) {
        if (action == GLFW_PRESS && key == GLFW_KEY_P)
            mypeline->reload();
    });
    main_renderer = std::make_unique<gfx::renderer>(1280, 720, 4);

    gfx::imgui imgui(main_window);

    const gl::sampler sampler;

    int w, h, c; stbi_info(gfx::file("indoor/posx.hdr").path.string().c_str(), &w, &h, &c);
    gl::texture cubemap(GL_TEXTURE_CUBE_MAP, w, h, GL_R11F_G11F_B10F);
    cubemap.assign(0, 0, 0, w, h, 1, GL_RGB, GL_FLOAT, gfx::image_file("indoor/posx.hdr", gfx::bits::b32, 3).bytes());
    cubemap.assign(0, 0, 1, w, h, 1, GL_RGB, GL_FLOAT, gfx::image_file("indoor/negx.hdr", gfx::bits::b32, 3).bytes());
    cubemap.assign(0, 0, 2, w, h, 1, GL_RGB, GL_FLOAT, gfx::image_file("indoor/posy.hdr", gfx::bits::b32, 3).bytes());
    cubemap.assign(0, 0, 3, w, h, 1, GL_RGB, GL_FLOAT, gfx::image_file("indoor/negy.hdr", gfx::bits::b32, 3).bytes());
    cubemap.assign(0, 0, 4, w, h, 1, GL_RGB, GL_FLOAT, gfx::image_file("indoor/posz.hdr", gfx::bits::b32, 3).bytes());
    cubemap.assign(0, 0, 5, w, h, 1, GL_RGB, GL_FLOAT, gfx::image_file("indoor/negz.hdr", gfx::bits::b32, 3).bytes());
    cubemap.generate_mipmaps();
    
    gfx::camera cam;
    gfx::camera_controller cam_controller(main_window);
    cam.transform.position = glm::vec3(0, 0, 5);
    
    mypeline = std::make_unique<gl::pipeline>();
    mypeline->at(GL_VERTEX_SHADER) = std::make_shared<gl::shader>("postfx/screen.vert");
    mypeline->at(GL_FRAGMENT_SHADER) = std::make_shared<gl::shader>("dist_field.frag");

    while(main_window->update())
    {
        imgui.new_frame();
        ImGui::Begin("Bla");
        ImGui::Value("DT", 1000 * static_cast<float>(main_window->delta_time()));
        ImGui::End();
    
        cam_controller.update(cam);

        main_renderer->bind();

        mypeline->bind();
        mypeline->at(GL_FRAGMENT_SHADER)->uniform<glm::mat4>("view_mat") = cam.view();
        mypeline->at(GL_FRAGMENT_SHADER)->uniform<glm::vec3>("cam_position") = cam.transform.position;
        mypeline->at(GL_FRAGMENT_SHADER)->uniform<glm::mat4>("proj_mat") = cam.projection();
        mypeline->at(GL_FRAGMENT_SHADER)->uniform<glm::mat4>("inv_view_mat") = inverse(cam.projection() * cam.view());
        mypeline->at(GL_FRAGMENT_SHADER)->uniform<uint64_t>("cubemap") = sampler.sample(cubemap);
        mypeline->draw(GL_TRIANGLES, 3);

        main_renderer->draw(main_window->delta_time());
        imgui.render();
    }
}