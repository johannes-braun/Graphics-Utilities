#include <io/window.hpp>
#include <io/camera.hpp>
#include <framework/renderer.hpp>
#include <framework/gizmo.hpp>
#include <memory>
#include <res/image.hpp>
#include <res/presets.hpp>
#include <res/geometry.hpp>

std::unique_ptr<io::window> window;
std::unique_ptr<gfx::renderer> renderer;

constexpr int start_width = 1280;
constexpr int start_height = 720;
constexpr int start_samples = 8;
constexpr float start_framerate = 120.f;
const glm::vec3 background = { 0.8f, 0.94f, 1.f };

void main()
{
    const res::image logo = res::load_svg_rasterized("../res/ui/logo.svg", 10.f);
    const res::image cursor = load_image("../res/cursor.png", res::image_type::u8, res::image_components::rgb_alpha);

    gl::shader::set_include_directories("../shaders");
    glfwWindowHint(GLFW_SAMPLES, start_samples);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    window = std::make_unique<io::window>(io::api::opengl, start_width, start_height, "Simple Rendering");
    window->set_icon(logo.width, logo.height, logo.data.get());
    window->set_cursor(new io::cursor(cursor.width, cursor.height, cursor.data.get(), 0, 0));
    window->set_max_framerate(start_framerate);
    window->callbacks->framebuffer_size_callback.add([](GLFWwindow*, int x, int y) {
        renderer->resize(x, y, start_samples);
        glViewport(0, 0, x, y);
    });
    renderer = std::make_unique<gfx::renderer>(start_width, start_height, start_samples);
    renderer->set_clear_color(glm::vec4(background, 1.f));

    io::camera camera;
    io::default_cam_controller controller;

    const auto scene = res::load_geometry("../res/bunny.dae");
    const auto vbo = jpu::make_ref<gl::buffer>(scene.meshes.get_by_index(0).vertices);
    const auto ibo = jpu::make_ref<gl::buffer>(scene.meshes.get_by_index(0).indices);

    const auto pipeline = jpu::make_ref<gl::graphics_pipeline>();
    pipeline->use_stages(new gl::shader("simple_gl/simple.vert"), new gl::shader("simple_gl/simple.frag"));
    pipeline->set_input_format(0, 3, GL_FLOAT, false);
    pipeline->set_input_format(1, 2, GL_FLOAT, false);
    pipeline->set_input_format(3, 3, GL_FLOAT, true);

    const auto image_texture = jpu::make_ref<gl::texture>(gl::texture_type::simple2d);
    const res::image image_texture_content = load_image("../res/bricks/brick.png", res::image_type::u8, res::image_components::rgb_alpha);
    image_texture->storage_2d(image_texture_content.width, image_texture_content.height, GL_RGBA8);
    image_texture->assign_2d(GL_RGBA, GL_UNSIGNED_BYTE, image_texture_content.data.get());
    image_texture->generate_mipmaps();
    const auto sampler = jpu::make_ref<gl::sampler>();

    glEnable(GL_DEPTH_TEST);

    res::transform light_transform;
    light_transform.position = { 4, 3, 4 };
    glm::vec3 light_color(100, 100, 100);
    
    gfx::gizmo gizmo;
    gizmo.transform = &light_transform;

    while (window->update())
    {
        controller.update(camera, *window, window->delta_time());

        ImGui::Begin("My Window");
        ImGui::ColorEdit3("Light Color", &light_color[0], ImGuiColorEditFlags_HDR);
        ImGui::End();

        double mx, my; glfwGetCursorPos(*window, &mx, &my);
        int sx, sy; glfwGetFramebufferSize(*window, &sx, &sy);
        gizmo.update(camera.view(), camera.projection(), glfwGetMouseButton(*window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS, mx / sx, my / sy);

        renderer->bind();
        pipeline->bind();
        pipeline->set_input_buffer(0, vbo, sizeof(res::vertex), offsetof(res::vertex, position));
        pipeline->set_input_buffer(1, vbo, sizeof(res::vertex), offsetof(res::vertex, uv));
        pipeline->set_input_buffer(2, vbo, sizeof(res::vertex), offsetof(res::vertex, normal));
        pipeline->set_index_buffer(ibo, gl::index_type::u32);

        pipeline->get_uniform<glm::mat4>(gl::shader_type::vertex, "view_projection") = camera.projection() * camera.view();
        pipeline->get_uniform<glm::mat4>(gl::shader_type::vertex, "model") = glm::mat4(1.f);

        pipeline->get_uniform<uint64_t>(gl::shader_type::fragment, "image_texture") = sampler->sample_texture(image_texture);
        pipeline->get_uniform<glm::vec3>(gl::shader_type::fragment, "background") = background;
        pipeline->get_uniform<glm::vec3>(gl::shader_type::fragment, "light_position") = light_transform.position;
        pipeline->get_uniform<glm::vec3>(gl::shader_type::fragment, "light_color") = light_color;

        pipeline->draw_indexed(gl::primitive::triangles, ibo->size() / sizeof(uint32_t));
        renderer->draw(window->delta_time());

        gizmo.render();
    }
}