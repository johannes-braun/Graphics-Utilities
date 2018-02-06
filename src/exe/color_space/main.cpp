#define IO_API_OPENGL

#include <jpu/memory>
#include <jpu/data>
#include "io/window.hpp"
#include "io/camera.hpp"
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include "res/image.hpp"
#include "res/presets.hpp"

jpu::ref_ptr<io::window> main_window;
jpu::named_vector<std::string, jpu::ref_ptr<gl::graphics_pipeline>> graphics_pipelines;
bool is_iconified = false;

glm::mat4 principal_axis_transformation(glm::u8vec3* pixels, size_t count)
{
    using namespace glm;
    vec3 mu = vec3(0);

    for (int i = 0; i<count; ++i)
        mu += vec3(pixels[i]);

    mu /= count * 255;
    float inv = 1 / 255.f;

    mat4 tr0 = mat4(1.0);
    tr0[3] = vec4(-mu, 1);

    mat4 tr1 = mat4(1.0);
    tr1[3] = vec4(normalize(vec3(1)) * length(mu), 1);

    mat3 rot(0);
    for (int i = 0; i<count; ++i)
    {
        const vec3 diff = vec3(pixels[i]) * inv - mu;
        rot += outerProduct(diff, diff);
    }
    rot /= count;

    vec3 ha = normalize(rot[0]);
    float len = length(rot[0]);

    if (length(rot[1]) > len)
    {
        len = length(rot[1]);
        ha = rot[1];
    }
    if (length(rot[2]) > len)
    {
        ha = rot[2];
    }

    ha = normalize(ha);
    float angle = dot(ha, normalize(vec3(1)));
    vec3 axis = cross(ha, normalize(vec3(1)));
    return tr1 * toMat4(angleAxis(angle, axis)) * tr0;
}

void main(int argc, const char** argv)
{
    gl::setup_shader_paths("../shaders");

    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    main_window = jpu::make_ref<io::window>(io::api::opengl, 1280, 720, "My Window");
    main_window->load_icon("../res/ui/logo.png");
    main_window->limit_framerate(60.0);

    GLFWimage img;
    auto storage = res::stbi_data(stbi_load("../res/cursor.png", &img.width, &img.height, nullptr, STBI_rgb_alpha));
    img.pixels = static_cast<uint8_t*>(storage.get());
    auto cursor = glfwCreateCursor(&img, 0, 0);
    glfwSetCursor(*main_window, cursor);

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
    const auto data = res::stbi_data(stbi_load("../res/tinted.jpg", &iw, &ih, nullptr, 3));
    texture->storage_2d(iw, ih, GL_RGB8);
    texture->assign_2d(GL_RGB, GL_UNSIGNED_BYTE, data.get());
    texture->generate_mipmaps();

    glm::mat4 hat = principal_axis_transformation(reinterpret_cast<glm::u8vec3*>(data.get()), iw * ih);

    glm::u8vec3* data_cast = reinterpret_cast<glm::u8vec3*>(data.get());
    std::vector<glm::u8vec3> new_img(iw * ih);
    glm::vec3 val{ 0 };
    for(int x=0; x < iw; ++x)
    {
        for(int y = 0; y<ih; ++y)
        {
            auto idx = x + y * iw;
            new_img[idx] = glm::clamp(glm::vec3(hat * glm::vec4(glm::vec3(data_cast[x + y * iw]) / 255.f, 1)) * 255.f,glm::vec3(0), glm::vec3(255.f));
            val += glm::vec3(data_cast[x + y * iw]);
        }
    }
    val /= glm::vec3(255.f);
    val /= iw * ih;

    //stbi_write_jpg("../res/corrected.jpg", iw, ih, 3, new_img.data(), 95);

    const auto grid = res::load_texture("../res/grid.jpg", GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);

    auto points_pipeline = graphics_pipelines.push("Points Pipeline", jpu::make_ref<gl::graphics_pipeline>());
    points_pipeline->use_stages(
        new gl::shader("../shaders/color_space/points.vert"),
        new gl::shader("../shaders/color_space/points.frag")
    );

    auto center_pipeline = graphics_pipelines.push("Center Pipeline", jpu::make_ref<gl::graphics_pipeline>());
    center_pipeline->use_stages(
        new gl::shader("../shaders/color_space/center_point.vert"),
        new gl::shader("../shaders/color_space/center_point.frag")
    );

    auto gizmo_pipeline = graphics_pipelines.push("Gizmo Pipeline", jpu::make_ref<gl::graphics_pipeline>());
    gizmo_pipeline->use_stages(
        new gl::shader("../shaders/color_space/gizmo.vert"),
        new gl::shader("../shaders/color_space/gizmo.frag")
    );

    auto cube_pipeline = graphics_pipelines.push("Cube Pipeline", jpu::make_ref<gl::graphics_pipeline>());
    cube_pipeline->use_stages(
        new gl::shader("../shaders/color_space/cube.vert"),
        new gl::shader("../shaders/color_space/cube.frag")
    );

    auto img_pipeline = graphics_pipelines.push("Image Pipeline", jpu::make_ref<gl::graphics_pipeline>());
    img_pipeline->use_stages(
        new gl::shader("../shaders/postprocess/screen.vert"),
        new gl::shader("../shaders/color_space/image.frag")
    );

    const auto vbo = jpu::make_ref<gl::buffer>(res::presets::cube::vertices);
    const auto ibo = jpu::make_ref<gl::buffer>(res::presets::cube::indices);
    const auto cube_vao = jpu::make_ref<gl::vertex_array>();
    cube_vao->add_bindings({
        gl::vertex_attribute_binding(0, *vbo, 0, 4, GL_FLOAT, sizeof(res::vertex), offsetof(res::vertex, position), false),
        gl::vertex_attribute_binding(0, *vbo, 0, 2, GL_FLOAT, sizeof(res::vertex), offsetof(res::vertex, uv), false),
    });
    cube_vao->set_element_buffer(*ibo);


    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    //glDepthFunc(GL_GEQUAL);
    //glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
    glClearColor(0.2f, 0.2f, 0.2f, 1.f);
    //glClearDepth(0);
    io::camera cam;
    io::default_cam_controller cam_controller;
    cam.transform.position = glm::vec3(0, 0, 5);
    const auto vao = jpu::make_ref<gl::vertex_array>();
    const auto line_vao = jpu::make_ref<gl::vertex_array>();
    
    glLineWidth(4.f);

    while (main_window->update())
    {
        static glm::vec3 color_offset{ 0, 0, 0 };
        static glm::vec3 color_factor{ 1,1,1 };
        const auto id = sampler->sample_texture(texture.get());
        ImGui::Begin("Bla");
        ImGui::Value("DT", 1000.f * static_cast<float>(main_window->delta_time()));
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
        ImGui::Text("Additive color correction");
        ImGui::PopFont();
        ImGui::ColorEdit3("##col_add", &color_offset[0], ImGuiColorEditFlags_HDR);
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
        ImGui::Text("Multiplicative color correction");
        ImGui::PopFont();
        ImGui::ColorEdit3("##col_mul", &color_factor[0], ImGuiColorEditFlags_HDR);
        static bool hat_en = true;
        ImGui::Checkbox("Show HAT", &hat_en);
        ImGui::End();

        cam_controller.update(cam, *main_window, main_window->delta_time());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        points_pipeline->bind();
        points_pipeline->stage(gl::shader_type::vertex)->get_uniform<glm::mat4>("hat_mat") = hat_en ? hat : glm::mat4(1.0);
        points_pipeline->stage(gl::shader_type::vertex)->get_uniform<gl::sampler2D>("picture") = id;
        points_pipeline->stage(gl::shader_type::vertex)->get_uniform<glm::mat4>("view_projection") = cam.projection() * cam.view();
        vao->bind();
        glDrawArrays(GL_POINTS, 0, texture->width() * texture->height());

        glDisable(GL_DEPTH_TEST);
        center_pipeline->bind();
        center_pipeline->stage(gl::shader_type::vertex)->get_uniform<glm::mat4>("hat_mat") = hat_en ? hat : glm::mat4(1.0);
        center_pipeline->stage(gl::shader_type::vertex)->get_uniform<glm::mat4>("view_projection") = cam.projection() * cam.view();
        center_pipeline->stage(gl::shader_type::vertex)->get_uniform<glm::vec3>("center") = val;
        glDrawArrays(GL_POINTS, 0, 1);
        glEnable(GL_DEPTH_TEST);

        gizmo_pipeline->bind();
        gizmo_pipeline->stage(gl::shader_type::vertex)->get_uniform<glm::mat4>("view_projection") = cam.projection() * cam.view();
        vao->bind();
        glDrawArrays(GL_LINES, 0, 6);

        glFrontFace(GL_CW);
        cube_pipeline->bind();
        cube_pipeline->stage(gl::shader_type::vertex)->get_uniform<glm::mat4>("view_projection") = cam.projection() * cam.view();
        cube_pipeline->stage(gl::shader_type::fragment)->get_uniform<gl::sampler2D>("tex") = sampler->sample_texture(grid.get());
        cube_pipeline->stage(gl::shader_type::fragment)->get_uniform<glm::vec4>("tint") = glm::vec4(1, 1, 1, 1);
        cube_vao->bind();
        glDrawElements(GL_TRIANGLES, res::presets::cube::indices.size(), GL_UNSIGNED_INT, nullptr);
        glFrontFace(GL_CCW);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        cube_pipeline->stage(gl::shader_type::fragment)->get_uniform<glm::vec4>("tint") = glm::vec4(1, 1, 1, -1);
        glDrawElements(GL_TRIANGLES, res::presets::cube::indices.size(), GL_UNSIGNED_INT, nullptr);
        glDisable(GL_BLEND);

        glDisable(GL_DEPTH_TEST);
        glViewport(0, 0, texture->width() / 5.f, texture->height() / 5.f);
        img_pipeline->bind();
        img_pipeline->stage(gl::shader_type::fragment)->get_uniform<glm::mat4>("hat_mat") = hat_en ? hat : glm::mat4(1.0);
        img_pipeline->stage(gl::shader_type::fragment)->get_uniform<gl::sampler2D>("tex") = id;
        vao->bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);

        int w, h;
        glfwGetFramebufferSize(*main_window, &w, &h);
        glViewport(0, 0, w, h);
        glEnable(GL_DEPTH_TEST);
    }
}