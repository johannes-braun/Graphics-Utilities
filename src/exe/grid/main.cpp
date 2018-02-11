#include <jpu/memory>
#include <jpu/flags>
#include "io/window.hpp"
#include "io/camera.hpp"
#include "res/vertex.hpp"
#include "res/presets.hpp"
#include "res/geometry.hpp"
#include "res/image.hpp"
#include "renderer.hpp"

jpu::ref_ptr<io::window> main_window;
jpu::ref_ptr<gl::graphics_pipeline> floor_pipeline;
std::unique_ptr<renderer> main_renderer;

jpu::named_vector<std::string, jpu::ref_ptr<gl::graphics_pipeline>> graphics_pipelines;

std::vector<res::vertex> floor_vertices;
std::vector<uint32_t> floor_indices;
void draw_gui();

int main(int argc, const char** args)
{
    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    main_window = jpu::make_ref<io::window>(io::api::opengl, 1280, 720, "Grid");
    main_window->load_icon("../res/ui/logo.png");
    main_window->set_cursor(new io::cursor("../res/cursor.png", 0, 0));
    main_window->limit_framerate(60.f);
    gl::setup_shader_paths("../shaders");
    main_renderer = std::make_unique<renderer>(1280, 720, 4);

    glfwSetKeyCallback(*main_window, [](GLFWwindow*, int key, int, int action, int mods) {
        if (main_window->gui()->key_action(key, action, mods))
            return;
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

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDepthFunc(GL_GEQUAL);
    glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
    glClearColor(0.8f, 0.9f, 1.f, 0);
    glClearDepth(0);

    io::camera camera;
    camera.inverse_z = true;
    io::default_cam_controller controller;

    struct mesh_info { std::vector<res::vertex> vtx; std::vector<uint32_t> idx; };
    enum mesh_pos { center = 0, side, corner, wall, mesh_count };

    std::vector<mesh_info> infos(mesh_count);

    {
        auto scene = res::load_geometry("../res/grid/center.dae");
        infos[center].vtx = std::move(scene.meshes.get_by_index(0).vertices);
        infos[center].idx = std::move(scene.meshes.get_by_index(0).indices);

        scene = res::load_geometry("../res/grid/border_straight.dae");
        infos[side].vtx = std::move(scene.meshes.get_by_index(0).vertices);
        infos[side].idx = std::move(scene.meshes.get_by_index(0).indices);

        scene = res::load_geometry("../res/grid/corner.dae");
        infos[corner].vtx = std::move(scene.meshes.get_by_index(0).vertices);
        infos[corner].idx = std::move(scene.meshes.get_by_index(0).indices);

        scene = res::load_geometry("../res/grid/wall.dae");
        infos[wall].vtx = std::move(scene.meshes.get_by_index(0).vertices);
        infos[wall].idx = std::move(scene.meshes.get_by_index(0).indices);
    }

    auto img = res::load_image("../res/heights.png", res::image_type::signed_float, res::image_components::grey);
    int width = img.width;
    int height = img.height;
    std::vector<int32_t> heights(width * height);
    std::vector<bool> blocked(width * height);
    for(int i=0; i<width*height; ++i)
    {
        if (static_cast<float*>(img.data.get())[i] > 0)
        {
            heights[i] = 1;
        }
        else
            heights[i] = 0;
    }

    const std::array<glm::ivec2, 8> offsets{
        glm::ivec2{1, 0},
        {1, 1},
        { 0, 1 },
        { -1, 1 },
        { -1, 0 },
        { -1, -1 },
        { 0, -1 },
        { 1, -1 }
    };

    for (int x = 0; x < width; ++x)
    {
        for (int z = 0; z < height; ++z)
        {
            std::array<int, 8> height_differences{ 0, 0, 0, 0, 0, 0, 0, 0 };
            for (int i = 0; i < offsets.size(); ++i)
            {
                const int dx = offsets[i].x;
                const int dz = offsets[i].y;

                if (x - dx < 0 || x - dx >= width || z + dz < 0 || z + dz >= height) continue;

                const int diff = static_cast<int>(heights[z * width + x]) - static_cast<int>(heights[(z + dz) * width + x - dx]);
                if (diff > 0)
                {
                    height_differences[i] = diff;
                }
            }

            for (int shift = -1; shift < 8; ++shift)
            {
                int rot;
                mesh_pos pos;

                if (shift == -1)
                {
                    rot = 0;
                    pos = center;
                }
                else if (height_differences[shift] > 0)
                {
                    /*const auto mask_fit = [&](int off) {
                        return height_differences[shift + off] != height_differences[shift];
                    };
                    
                    if (shift % 2 == 1 && (height_differences[shift + 1] != 1 || height_differences[shift -1] != 1))
                    {
                        continue;
                    }*/

                    rot = shift / 2;
                    pos = mesh_pos(1 + shift % 2);
                }
                else continue;

                if (shift % 2 == 0 && height_differences[shift] > 1)
                {
                    for (int h = 0; h < height_differences[shift] - 1   ; ++h)
                    {
                       // log_i << h << height_differences[shift];
                        const glm::mat4 transform = glm::translate(glm::vec3(2.f*x, heights[z * width + x] - h, 2.f*z)) * glm::rotate(glm::radians(90.f) * static_cast<float>(rot), glm::vec3(0, 1, 0));
                        const size_t old_vtx_size = floor_vertices.size();
                        const mesh_pos w = wall;
                        floor_vertices.resize(old_vtx_size + infos[w].vtx.size());
                        for (int i = 0; i < infos[w].vtx.size(); ++i)
                        {
                            auto vertex = infos[w].vtx[i];
                            vertex.position = transform * vertex.position;
                            floor_vertices[old_vtx_size + i] = vertex;
                        }

                        const size_t old_idx_size = floor_indices.size();
                        floor_indices.resize(old_idx_size + infos[w].idx.size());
                        for (int i = 0; i < infos[w].idx.size(); ++i)
                        {
                            auto idx = infos[w].idx[i];
                            idx += old_vtx_size;
                            floor_indices[old_idx_size + i] = idx;
                        }
                    }
                }
                
                const glm::mat4 transform = glm::translate(glm::vec3(2.f*x, heights[z * width + x], 2.f*z)) * glm::rotate(glm::radians(90.f) * static_cast<float>(rot), glm::vec3(0, 1, 0));

                const size_t old_vtx_size = floor_vertices.size();
                floor_vertices.resize(old_vtx_size + infos[pos].vtx.size());
                for (int i = 0; i < infos[pos].vtx.size(); ++i)
                {
                    auto vertex = infos[pos].vtx[i];
                    vertex.position = transform * vertex.position;
                    floor_vertices[old_vtx_size + i] = vertex;
                }

                const size_t old_idx_size = floor_indices.size();
                floor_indices.resize(old_idx_size + infos[pos].idx.size());
                for (int i = 0; i < infos[pos].idx.size(); ++i)
                {
                    auto idx = infos[pos].idx[i];
                    idx += old_vtx_size;
                    floor_indices[old_idx_size + i] = idx;
                }
            }
        }
    }

    //load mesh
    //floor_vertices = { res::presets::cube::vertices.begin(), res::presets::cube::vertices.end() };
    //floor_indices = { res::presets::cube::indices.begin(), res::presets::cube::indices.end() };
    const auto vertex_buffer = jpu::make_ref<gl::buffer>(floor_vertices);
    const auto element_buffer = jpu::make_ref<gl::buffer>(floor_indices);
    const auto vertex_array = jpu::make_ref<gl::vertex_array>();
    vertex_array->add_bindings({
        gl::vertex_attribute_binding(0, *vertex_buffer, 0, 3, GL_FLOAT, sizeof(res::vertex), offsetof(res::vertex, position), false),
        gl::vertex_attribute_binding(1, *vertex_buffer, 0, 2, GL_FLOAT, sizeof(res::vertex), offsetof(res::vertex, uv), false),
        gl::vertex_attribute_binding(2, *vertex_buffer, 0, 3, GL_FLOAT, sizeof(res::vertex), offsetof(res::vertex, normal), true)
        });
    vertex_array->set_element_buffer(*element_buffer);

    // Load simple mesh shader
    floor_pipeline = jpu::make_ref<gl::graphics_pipeline>();
    floor_pipeline->use_stages(
        new gl::shader(gl::shader_root_path / "grid/vs.vert"),
        new gl::shader(gl::shader_root_path / "grid/fs.frag")
    );

    while (main_window->update())
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        draw_gui();
        controller.update(camera, *main_window, main_window->delta_time());

        main_renderer->bind();
        floor_pipeline->bind();
        floor_pipeline->stage(gl::shader_type::vertex)->get_uniform<glm::mat4>("view_projection") = camera.projection() * camera.view();
        floor_pipeline->stage(gl::shader_type::fragment)->get_uniform<glm::mat4>("inv_view") = inverse(camera.view());
        floor_pipeline->stage(gl::shader_type::fragment)->get_uniform<float>("time") = glfwGetTime();
        vertex_array->bind();
        glDrawElements(GL_TRIANGLES, floor_indices.size(), GL_UNSIGNED_INT, nullptr);
        main_renderer->draw(main_window->delta_time());
    }
    return 0;
}

void draw_gui()
{
    ImGui::Begin("Settings");
    ImGui::Value("Delta-Time", static_cast<float>(main_window->delta_time()));
    if (ImGui::Button("Reload Pipeline"))
        floor_pipeline->reload_stages();
    ImGui::End();
}