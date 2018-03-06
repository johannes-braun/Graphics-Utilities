#include <io/window.hpp>
#include <io/camera.hpp>
#include <framework/renderer.hpp>
#include <framework/gizmo.hpp>
#include <memory>
#include <res/image.hpp>
#include <res/geometry.hpp>
#include <opengl/debug.hpp>
#include <opengl/command_list.hpp>
#include <opengl/state.hpp>

std::unique_ptr<io::window> window;
std::unique_ptr<gfx::renderer> renderer;

constexpr int start_width = 1280;
constexpr int start_height = 720;
constexpr int start_samples = 8;
constexpr float start_framerate = 120.f;
const glm::vec3 background = { 0.8f, 0.94f, 1.f };

int main()
{
    const res::image logo = res::load_svg_rasterized("../res/ui/logo.svg", 10.f);
    const res::image cursor = load_image("../res/cursor.png", res::image_type::u8, res::image_components::rgb_alpha);

    gl::shader::set_include_directories("../shaders");
    glfwWindowHint(GLFW_SAMPLES, start_samples);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    window = std::make_unique<io::window>(io::api::opengl, start_width, start_height, "Simple Rendering");
    window->set_icon(logo.width, logo.height, logo.data.get());
    window->set_cursor(new io::cursor(cursor.width, cursor.height, cursor.data.get(), 0, 0));
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

    const auto image_texture = jpu::make_ref<gl::texture>(gl::texture_type::simple2d);
    const res::image image_texture_content = load_image("../res/bricks/brick.png", res::image_type::u8, res::image_components::rgb_alpha);
    image_texture->storage_2d(image_texture_content.width, image_texture_content.height, GL_RGBA8);
    image_texture->assign_2d(GL_RGBA, GL_UNSIGNED_BYTE, image_texture_content.data.get());
    image_texture->generate_mipmaps();
    const auto sampler = jpu::make_ref<gl::sampler>();

    res::transform light_transform;
    light_transform.position = { 4, 3, 4 };
    glm::vec3 light_color(100, 100, 100);

    gfx::gizmo gizmo;
    gizmo.transform = &light_transform;

    struct uniforms_1
    {
        alignas(16) glm::mat4 view_projection;
        alignas(16) glm::mat4 model;
    };
    struct uniforms_2
    {
        alignas(16) uint64_t image_texture;
        alignas(16) glm::vec3 light_position;
        alignas(16) glm::vec3 light_color;
        alignas(16) glm::vec3 background;
    };

    gl::buffer uniform_buffer1(uniforms_1{}, gl::buffer_flag_bits::map_dynamic_persistent_write);
    gl::buffer uniform_buffer2(uniforms_2{}, gl::buffer_flag_bits::map_dynamic_persistent_write);
    uniforms_1* u_data1 = uniform_buffer1.data_as<uniforms_1>();
    uniforms_2* u_data2 = uniform_buffer2.data_as<uniforms_2>();

    gl::state state;
    renderer->bind();
    pipeline->bind();
    glViewport(0, 0, start_width, start_height);
    glScissor(0, 0, start_width, start_height);
    glEnable(GL_DEPTH_TEST);
    const auto add_attrib = [](uint32_t a, uint32_t c, uint32_t t, bool n, size_t s)
    {
        glEnableVertexAttribArray(a); 
        glVertexAttribFormat(a, c, t, n, 0); 
        glVertexAttribBinding(a, a);
        glBindVertexBuffer(a, 0, 0, s);
    };
    add_attrib(0, 3, GL_FLOAT, false, sizeof(res::vertex));
    add_attrib(1, 3, GL_FLOAT, false, sizeof(res::vertex));
    add_attrib(2, 2, GL_FLOAT, false, sizeof(res::vertex));
    state.capture(gl::basic_primitive::triangles);

    gl::command_buffer command_buffer;
    command_buffer.begin();
    command_buffer.push(gl::cmd_attribute_address{ 0, vbo->address() + offsetof(res::vertex, position) });
    command_buffer.push(gl::cmd_attribute_address{ 1, vbo->address() + offsetof(res::vertex, normal) });
    command_buffer.push(gl::cmd_attribute_address{ 2, vbo->address() + offsetof(res::vertex, uv) });
    command_buffer.push(gl::cmd_element_address{ ibo->address(), sizeof(uint32_t) });
    command_buffer.push(gl::cmd_uniform_address{ 0, gl::shader_type::vertex, uniform_buffer1.address() });
    command_buffer.push(gl::cmd_uniform_address{ 0, gl::shader_type::fragment, uniform_buffer2.address() });
    command_buffer.push(gl::cmd_draw_elements{ static_cast<uint32_t>(ibo->size() / sizeof(uint32_t)), 0, 0 });
    command_buffer.end();

    while (window->update())
    {
        controller.update(camera, *window, window->delta_time());

        ImGui::Begin("My Window");
        ImGui::ColorEdit3("Light Color", &light_color[0], ImGuiColorEditFlags_HDR);
        ImGui::Value("Time", 1 / static_cast<float>(window->delta_time()));
        ImGui::End();

        double mx, my; glfwGetCursorPos(*window, &mx, &my);
        int sx, sy; glfwGetFramebufferSize(*window, &sx, &sy);
        gizmo.update(camera.view(), camera.projection(), glfwGetMouseButton(*window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS, mx / sx, my / sy);

        u_data1->view_projection = camera.projection() * camera.view();
        u_data1->model = glm::mat4(1.f);

        u_data2->image_texture = sampler->sample_texture(image_texture);
        u_data2->background = background;
        u_data2->light_position = light_transform.position;
        u_data2->light_color = light_color;

        renderer->bind();
        uint32_t fbos = *renderer->main_framebuffer();
        uint32_t states = state;
        glDrawCommandsStatesAddressNV(&command_buffer.indirect, &command_buffer.size, &states, &fbos, 1);
        renderer->draw(window->delta_time());

        gizmo.render();
    }
}