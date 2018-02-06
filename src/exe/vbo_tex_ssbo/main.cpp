#define IO_API_OPENGL

#include <jpu/memory>
#include <jpu/data>
#include "io/window.hpp"
#include "io/camera.hpp"
#include "stb/stb_image.h"
#include "res/image.hpp"

jpu::ref_ptr<io::window> main_window;
jpu::named_vector<std::string, jpu::ref_ptr<gl::graphics_pipeline>> graphics_pipelines;

void main(int argc, const char** argv)
{
    gl::setup_shader_paths("../shaders");

    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    main_window = jpu::make_ref<io::window>(io::api::opengl, 1280, 720, "My Window");
    main_window->load_icon("../res/ui/logo.png");
    glfwSwapInterval(0);
    // glfwSwapInterval(1);
    glfwSetKeyCallback(*main_window, [](GLFWwindow*, int key, int, int action, int mods) {
        if (main_window->gui()->key_action(key, action, mods))
            return;

        if (action == GLFW_PRESS && key == GLFW_KEY_P)
            for (auto&& p : graphics_pipelines)
                p->reload_stages();
    });

    glfwSetScrollCallback(*main_window, [](GLFWwindow*, double x, double y) {
        main_window->gui()->scrolled(y);
    });

    glfwSetCharCallback(*main_window, [](GLFWwindow*, uint32_t ch) {
        if (main_window->gui()->char_input(static_cast<wchar_t>(ch)))
            return;
    });

    glfwSetMouseButtonCallback(*main_window, [](GLFWwindow*, int btn, int action, int mods) {
        if (main_window->gui()->mouse_button_action(btn, action, mods))
            return;
    });

    const auto sampler = jpu::make_ref<gl::sampler>();
    sampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    sampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    sampler->set(GL_TEXTURE_MAX_ANISOTROPY, 16);
    sampler->set(GL_TEXTURE_CUBE_MAP_SEAMLESS, 16);
    sampler->set(GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
    sampler->set(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    sampler->set(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    auto texture = jpu::make_ref<gl::texture>(gl::texture_type::def_2d);
    int iw, ih;
    const auto data = res::stbi_data(stbi_loadf("../res/img.jpg", &iw, &ih, nullptr, 4));
    texture->storage_2d(iw, ih, GL_RGBA32F);
    texture->assign_2d(GL_RGBA, GL_FLOAT, data.get());
    texture->generate_mipmaps();

    auto texture_buf = jpu::make_ref<gl::texture>(gl::texture_type::buffer);

    auto vbo_pipeline = graphics_pipelines.push("VBO Pipeline", jpu::make_ref<gl::graphics_pipeline>());
    vbo_pipeline->use_stages(
        new gl::shader("../shaders/vbo_tex_ssbo/vbo.vert"),
        new gl::shader("../shaders/vbo_tex_ssbo/draw.frag")
    );

    auto ssbo_pipeline = graphics_pipelines.push("VBO Pipeline", jpu::make_ref<gl::graphics_pipeline>());
    ssbo_pipeline->use_stages(
        new gl::shader("../shaders/vbo_tex_ssbo/ssbo.vert"),
        new gl::shader("../shaders/vbo_tex_ssbo/draw.frag")
    );

    auto tex_pipeline = graphics_pipelines.push("Tex Pipeline", jpu::make_ref<gl::graphics_pipeline>());
    tex_pipeline->use_stages(
        new gl::shader("../shaders/vbo_tex_ssbo/tex.vert"),
        new gl::shader("../shaders/vbo_tex_ssbo/draw.frag")
    );
    tex_pipeline->stage(gl::shader_type::vertex)->get_uniform<gl::sampler2D>("picture") = sampler->sample_texture(texture);

    const auto vbo = jpu::make_ref<gl::buffer>(static_cast<glm::vec4*>(data.get()), iw*ih);
    const auto vbo_vao = jpu::make_ref<gl::vertex_array>();
    vbo_vao->add_bindings({
        gl::vertex_attribute_binding(0, *vbo, 0, 4, GL_FLOAT, sizeof(glm::vec4), 0, false),
        });

   // vbo->bind(0, GL_SHADER_STORAGE_BUFFER);
    texture_buf->set_buffer(*vbo, GL_RGBA32F);

    auto tex_buf_pipeline = graphics_pipelines.push("Tex Buf Pipeline", jpu::make_ref<gl::graphics_pipeline>());
    tex_buf_pipeline->use_stages(
        new gl::shader("../shaders/vbo_tex_ssbo/tex_buf.vert"),
        new gl::shader("../shaders/vbo_tex_ssbo/draw.frag")
    );
    tex_buf_pipeline->stage(gl::shader_type::vertex)->get_uniform<gl::sampler2D>("picture") = texture_buf->address();

    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClearColor(0.2f, 0.2f, 0.2f, 1.f);

    io::camera cam;
    io::default_cam_controller cam_controller;
    cam.transform.position = glm::vec3(0, 0, 5);
    const auto vao = jpu::make_ref<gl::vertex_array>();

    glLineWidth(4.f);

    while (main_window->update())
    {
        static int sel = 0;
        ImGui::Begin("Bla");
        ImGui::Value("DT", 1000.f * static_cast<float>(main_window->delta_time()));
        ImGui::Value("Amount", iw * ih);
        ImGui::Combo("Mode", &sel, " VBO \0 Tex \0 SSBO \0 Buf Texture");
        ImGui::End();
        cam_controller.update(cam, *main_window, main_window->delta_time());

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        switch (sel)
        {
        case 0:
        {
            vbo_pipeline->bind();
            vbo_pipeline->stage(gl::shader_type::vertex)->get_uniform<glm::mat4>("view_projection") = cam.projection() * cam.view();
            vbo_vao->bind();
            glDrawArrays(GL_POINTS, 0, iw * ih);
        } break;
        case 1:
        {
            tex_pipeline->bind();
            tex_pipeline->stage(gl::shader_type::vertex)->get_uniform<glm::mat4>("view_projection") = cam.projection() * cam.view();
            vao->bind();
            glDrawArrays(GL_POINTS, 0, iw * ih);
        } break;
        case 2:
        {
            ssbo_pipeline->bind();
            ssbo_pipeline->stage(gl::shader_type::vertex)->get_uniform<glm::mat4>("view_projection") = cam.projection() * cam.view();
            vao->bind();
            glDrawArrays(GL_POINTS, 0, iw * ih);
        } break;
        case 3:
        {
            tex_buf_pipeline->bind();
            tex_buf_pipeline->stage(gl::shader_type::vertex)->get_uniform<glm::mat4>("view_projection") = cam.projection() * cam.view();
            vao->bind();
            glDrawArrays(GL_POINTS, 0, iw * ih);
        } break;

        default: break;
        }
    }
}