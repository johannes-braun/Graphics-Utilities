#include <jpu/memory.hpp>
#include <jpu/data.hpp>
#include "io/window.hpp"
#include "io/camera.hpp"
#include "stb_image.h"
#include "res/image.hpp"
#include "framework/renderer.hpp"
#include "framework/gizmo.hpp"

std::unique_ptr<gfx::renderer> main_renderer;
std::unique_ptr<io::window> main_window;
jpu::named_vector<std::string, jpu::ref_ptr<gl::graphics_pipeline>> graphics_pipelines;

int main(int argc, const char** argv)
{
    gl::shader::set_include_directories("../shaders");

    res::image icon = load_image("../res/ui/logo.png", res::image_type::u8, res::image_components::rgb_alpha);
    res::image cursor = load_image("../res/cursor.png", res::image_type::u8, res::image_components::rgb_alpha);

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    main_window = std::make_unique<io::window>(io::api::opengl, 1280, 720, "My Window");
    main_window->set_icon(icon.width, icon.height, icon.data.get());
    main_window->set_cursor(new io::cursor(cursor.width, cursor.height, cursor.data.get(), 0, 0));
    main_window->set_max_framerate(60.f);
    main_window->callbacks->key_callback.add([](GLFWwindow*, int key, int, int action, int mods) {
        if (action == GLFW_PRESS && key == GLFW_KEY_P)
            for (auto&& p : graphics_pipelines)
                p->reload_stages();
    });
    main_renderer = std::make_unique<gfx::renderer>(1280, 720, 4);

    const gl::sampler sampler;

    int w, h, c; stbi_info("../res/ven/hdr/posx.hdr", &w, &h, &c);
    gl::v2::texture cubemap(GL_TEXTURE_CUBE_MAP, w, h, GL_R11F_G11F_B10F);
    cubemap.assign(0, 0, 0, w, h, 1, 0, GL_RGB, GL_FLOAT, res::stbi_data(stbi_loadf("../res/ven/hdr/posx.hdr", &c, &c, nullptr, STBI_rgb)).get());
    cubemap.assign(0, 0, 1, w, h, 1, 0, GL_RGB, GL_FLOAT, res::stbi_data(stbi_loadf("../res/ven/hdr/negx.hdr", &c, &c, nullptr, STBI_rgb)).get());
    cubemap.assign(0, 0, 2, w, h, 1, 0, GL_RGB, GL_FLOAT, res::stbi_data(stbi_loadf("../res/ven/hdr/posy.hdr", &c, &c, nullptr, STBI_rgb)).get());
    cubemap.assign(0, 0, 3, w, h, 1, 0, GL_RGB, GL_FLOAT, res::stbi_data(stbi_loadf("../res/ven/hdr/negy.hdr", &c, &c, nullptr, STBI_rgb)).get());
    cubemap.assign(0, 0, 4, w, h, 1, 0, GL_RGB, GL_FLOAT, res::stbi_data(stbi_loadf("../res/ven/hdr/posz.hdr", &c, &c, nullptr, STBI_rgb)).get());
    cubemap.assign(0, 0, 5, w, h, 1, 0, GL_RGB, GL_FLOAT, res::stbi_data(stbi_loadf("../res/ven/hdr/negz.hdr", &c, &c, nullptr, STBI_rgb)).get());
    cubemap.generate_mipmaps();

    auto distance_field_pipeline = graphics_pipelines.push("Distance Field Pipeline", jpu::make_ref<gl::graphics_pipeline>());
    distance_field_pipeline->use_stages(std::make_shared<gl::shader>("postprocess/screen.vert"), std::make_shared<gl::shader>("distance_field/dist_field.frag"));

    io::camera cam;
    io::default_cam_controller cam_controller;
    cam.transform.position = glm::vec3(0, 0, 5);
    
    while(main_window->update())
    {
        ImGui::Begin("Bla");
        ImGui::Value("DT", 1000 * static_cast<float>(main_window->delta_time()));
        ImGui::End();

        cam_controller.update(cam, *main_window, main_window->delta_time());

        main_renderer->bind();
        distance_field_pipeline->bind();
        distance_field_pipeline->get_uniform<glm::mat4>(GL_FRAGMENT_SHADER, "view_mat") = cam.view();
        distance_field_pipeline->get_uniform<glm::vec3>(GL_FRAGMENT_SHADER, "cam_position") = cam.transform.position;
        distance_field_pipeline->get_uniform<glm::mat4>(GL_FRAGMENT_SHADER, "proj_mat") = cam.projection();
        distance_field_pipeline->get_uniform<glm::mat4>(GL_FRAGMENT_SHADER, "inv_view_mat") = inverse(cam.projection() * cam.view());
        distance_field_pipeline->get_uniform<uint64_t>(GL_FRAGMENT_SHADER, "cubemap") = sampler.sample(cubemap);
        distance_field_pipeline->draw(gl::primitive::triangles, 3);
        main_renderer->draw(main_window->delta_time());
    }
}