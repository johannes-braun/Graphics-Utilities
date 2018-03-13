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
#include <opengl/texture.hpp>

std::unique_ptr<io::window> window;
std::unique_ptr<gfx::renderer> renderer;

constexpr int start_width = 800;
constexpr int start_height = 600;
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
        glViewportIndexedf(0, 0, 0, x, y);
    });
    renderer = std::make_unique<gfx::renderer>(start_width, start_height, start_samples);
    renderer->set_clear_color(glm::vec4(background, 1.f));

    io::camera camera;
    io::default_cam_controller controller;

    const auto scene = res::load_geometry("../res/bunny.dae");
    gl::buffer<res::vertex> vbo(scene.meshes.get_by_index(0).vertices.begin(), scene.meshes.get_by_index(0).vertices.end(), GL_DYNAMIC_STORAGE_BIT);
    gl::buffer<uint32_t> ibo(scene.meshes.get_by_index(0).indices.begin(), scene.meshes.get_by_index(0).indices.end(), GL_DYNAMIC_STORAGE_BIT);
    
    glm::mat4 trf = glm::rotate(glm::radians(90.f), glm::vec3(1, 0, 0));
    std::transform(vbo.begin(), vbo.end(), vbo.begin(), [&trf](res::vertex v) {
        v.position  = trf * glm::vec4(v.position, 1);
        return v;
    });
    vbo.clear();

    const auto pipeline = jpu::make_ref<gl::graphics_pipeline>();
    pipeline->use_stages(std::make_shared<gl::shader>("simple_gl/simple.vert"), std::make_shared<gl::shader>("simple_gl/simple.frag"));

    const res::image image_texture_content = load_image("../res/bricks/brick.png", res::image_type::u8, res::image_components::rgb_alpha);
    gl::v2::texture image_texture(GL_TEXTURE_2D, image_texture_content.width, image_texture_content.height, GL_RGBA8);
    image_texture.assign(GL_RGBA, GL_UNSIGNED_BYTE, image_texture_content.data.get());
    image_texture.generate_mipmaps();

    const gl::sampler sampler;

    res::transform light_transform;
    light_transform.position = { 4, 3, 4 };
    glm::vec3 light_color(100, 100, 100);

    gfx::gizmo gizmo;
    gizmo.transform = &light_transform;

    struct uniforms_1
    {
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 projection;
        alignas(16) glm::mat4 model;
    };
    struct uniforms_2
    {
        alignas(16) uint64_t image_texture;
        alignas(16) glm::vec3 light_position;
        alignas(16) glm::vec3 light_color;
        alignas(16) glm::vec3 background;
    };

    gl::buffer<uniforms_1> uniform_buffer1(1, GL_DYNAMIC_STORAGE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT);
    uniform_buffer1.map(GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT);
    gl::buffer<uniforms_2> uniform_buffer2(1, GL_DYNAMIC_STORAGE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT);
    uniform_buffer2.map(GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT);
    
    gl::state state;
    renderer->bind();
    pipeline->bind();
    glViewportIndexedf(0, 0, 0, start_width, start_height);
    glScissorIndexed(0, 0, 0, start_width, start_height);
    glEnable(GL_DEPTH_TEST);
    const auto add_attrib = [](uint32_t a, uint32_t c, GLenum t, bool n, size_t s)
    {
        glEnableVertexAttribArray(a); 
        glVertexAttribFormat(a, c, t, n, 0); 
        glVertexAttribBinding(a, a);
        glBindVertexBuffer(a, gl_buffer_t::zero, 0, s);
    };
    add_attrib(0, 3, GL_FLOAT, false, sizeof(res::vertex));
    add_attrib(1, 3, GL_FLOAT, false, sizeof(res::vertex));
    add_attrib(2, 2, GL_FLOAT, false, sizeof(res::vertex));
    state.capture(gl::basic_primitive::triangles);

    gl::command_buffer command_buffer;
    command_buffer.begin();
    command_buffer.push(gl::cmd_attribute_address{ 0, vbo.handle() + offsetof(res::vertex, position) });
    command_buffer.push(gl::cmd_attribute_address{ 1, vbo.handle() + offsetof(res::vertex, normal) });
    command_buffer.push(gl::cmd_attribute_address{ 2, vbo.handle() + offsetof(res::vertex, uv) });
    command_buffer.push(gl::cmd_element_address{ ibo.handle(), sizeof(uint32_t) });
    command_buffer.push(gl::cmd_uniform_address{ 0, GL_VERTEX_SHADER, uniform_buffer1.handle() });
    command_buffer.push(gl::cmd_uniform_address{ 0, GL_FRAGMENT_SHADER, uniform_buffer2.handle() });
    command_buffer.push(gl::cmd_draw_elements{ static_cast<uint32_t>(ibo.size()), 0, 0 });
    command_buffer.end();

    while (window->update())
    {
        controller.update(camera, *window, window->delta_time());

        ImGui::Begin("My Window");
        ImGui::ColorEdit3("Light Color", &light_color[0], ImGuiColorEditFlags_HDR);
        ImGui::Value("Time", 1 / static_cast<float>(window->delta_time()));
        if (ImGui::Button("Reload PP"))
        {
            renderer->reload_pipelines();
        }
        if (ImGui::Button("Assign data!"))
        {
            if(vbo.empty())
                vbo.insert(vbo.begin(), scene.meshes.get_by_index(0).vertices.begin(), scene.meshes.get_by_index(0).vertices.end());
            else
                vbo.assign(vbo.begin(), scene.meshes.get_by_index(0).vertices.begin(), scene.meshes.get_by_index(0).vertices.end());
        }

        ImGui::End();

        gizmo.update(*window, camera.view(), camera.projection());

        uniform_buffer1[0].view = camera.view();
        uniform_buffer1[0].projection = camera.projection();
        uniform_buffer1[0].model = glm::mat4(1.f);

        uniform_buffer2[0].image_texture = sampler.sample(image_texture);
        uniform_buffer2[0].background = background;
        uniform_buffer2[0].light_position = light_transform.position;
        uniform_buffer2[0].light_color = light_color;

        renderer->bind();
        gl_framebuffer_t fbos = *renderer->main_framebuffer();
        gl_state_nv_t states = state;
        glDrawCommandsStatesAddressNV(&command_buffer.indirect, &command_buffer.size, &states, &fbos, 1);
        renderer->draw(window->delta_time());

        gizmo.render();
    }
}