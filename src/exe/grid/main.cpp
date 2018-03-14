#include <jpu/memory.hpp>
#include <jpu/flags.hpp>
#include "io/window.hpp"
#include "io/camera.hpp"
#include "res/vertex.hpp"
#include "res/presets.hpp"
#include "res/geometry.hpp"
#include "res/image.hpp"
#include "framework/renderer.hpp"

jpu::ref_ptr<io::window> main_window;
std::unique_ptr<gl::pipeline> floor_pipeline;
std::unique_ptr<gfx::renderer> main_renderer;

std::vector<res::vertex> floor_vertices;
std::vector<uint32_t> floor_indices;
void draw_gui();

int main()
{
    gl::shader::set_include_directories("../shaders");

    res::image icon = load_image("../res/ui/logo.png", res::image_type::u8, res::RGBA);
    res::image cursor = load_image("../res/cursor.png", res::image_type::u8, res::RGBA);

    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    main_window = jpu::make_ref<io::window>(io::api::opengl, 1280, 720, "Grid");
    main_window->set_icon(icon.width, icon.height, icon.data.get());
    main_window->set_cursor(new io::cursor(cursor.width, cursor.height, cursor.data.get(), 0, 0));
    main_window->set_max_framerate(60.f);
    main_renderer = std::make_unique<gfx::renderer>(1280, 720, 4);

    main_window->callbacks->key_callback.add([](GLFWwindow*, int key, int, int action, int mods) {
        if (ImGui::GetIO().WantCaptureKeyboard || ImGui::GetIO().WantTextInput)
            return;

        if (key == GLFW_KEY_P && action == GLFW_PRESS)
        {
            floor_pipeline->reload();
        }
    });

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClearColor(0.8f, 0.9f, 1.f, 0);

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

    auto img = load_image("../res/grid/heightmap.png", res::image_type::f32, res::Grey);
    const int width = img.width;
    const int height = img.height;
    std::vector<int32_t> heights(width * height);
    std::vector<bool> blocked(width * height);

    constexpr float scale = 12.f;
    for(int i=0; i<width*height; ++i)
    {
        const float val = static_cast<float*>(img.data.get())[i];
        heights[i] = int(val > 0 ? (val * scale +1) : val);
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
                        const glm::mat4 transform = translate(glm::vec3(2.f*x, heights[z * width + x] - h, 2.f*z)) * rotate(glm::radians(90.f) * static_cast<float>(rot), glm::vec3(0, 1, 0));
                        const size_t old_vtx_size = floor_vertices.size();
                        const mesh_pos w = wall;
                        floor_vertices.resize(old_vtx_size + infos[w].vtx.size());
                        for (int i = 0; i < infos[w].vtx.size(); ++i)
                        {
                            auto vertex = infos[w].vtx[i];
                            vertex.position = transform * glm::vec4(vertex.position, 1);
                            floor_vertices[old_vtx_size + i] = vertex;
                        }

                        const size_t old_idx_size = floor_indices.size();
                        floor_indices.resize(old_idx_size + infos[w].idx.size());
                        for (int i = 0; i < infos[w].idx.size(); ++i)
                        {
                            auto idx = infos[w].idx[i];
                            idx += uint32_t(old_vtx_size);
                            floor_indices[old_idx_size + i] = idx;
                        }
                    }
                }
                
                const glm::mat4 transform = translate(glm::vec3(2.f*x, heights[z * width + x], 2.f*z)) * rotate(glm::radians(90.f) * static_cast<float>(rot), glm::vec3(0, 1, 0));

                const size_t old_vtx_size = floor_vertices.size();
                floor_vertices.resize(old_vtx_size + infos[pos].vtx.size());
                for (int i = 0; i < infos[pos].vtx.size(); ++i)
                {
                    auto vertex = infos[pos].vtx[i];
                    vertex.position = transform * glm::vec4(vertex.position, 1);
                    floor_vertices[old_vtx_size + i] = vertex;
                }

                const size_t old_idx_size = floor_indices.size();
                floor_indices.resize(old_idx_size + infos[pos].idx.size());
                for (int i = 0; i < infos[pos].idx.size(); ++i)
                {
                    auto idx = infos[pos].idx[i];
                    idx += uint32_t(old_vtx_size);
                    floor_indices[old_idx_size + i] = idx;
                }
            }
        }
    }

    gl::buffer<res::vertex> vertex_buffer(floor_vertices.begin(), floor_vertices.end());
    gl::buffer<uint32_t> element_buffer(floor_indices.begin(), floor_indices.end());

    // Load simple mesh shader
    floor_pipeline = std::make_unique<gl::pipeline>();
    floor_pipeline->at(GL_VERTEX_SHADER) = std::make_shared<gl::shader>("grid/vs.vert");
    floor_pipeline->at(GL_FRAGMENT_SHADER) = std::make_shared<gl::shader>("grid/fs.frag");

    gl::pipeline cubemap_pipeline;
    cubemap_pipeline[GL_VERTEX_SHADER] = std::make_shared<gl::shader>("cubemap/cubemap.vert");
    cubemap_pipeline[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("cubemap/cubemap.frag");

    const gl::sampler sampler;

    int w, h, c; stbi_info("../res/hdr/posx.hdr", &w, &h, &c);
    gl::texture cubemap(GL_TEXTURE_CUBE_MAP, w, h, GL_R11F_G11F_B10F);
    cubemap.assign(0, 0, 0, w, h, 1, GL_RGB, GL_FLOAT, res::stbi_data(stbi_loadf("../res/hdr/posx.hdr", &c, &c, nullptr, STBI_rgb)).get());
    cubemap.assign(0, 0, 1, w, h, 1, GL_RGB, GL_FLOAT, res::stbi_data(stbi_loadf("../res/hdr/negx.hdr", &c, &c, nullptr, STBI_rgb)).get());
    cubemap.assign(0, 0, 2, w, h, 1, GL_RGB, GL_FLOAT, res::stbi_data(stbi_loadf("../res/hdr/posy.hdr", &c, &c, nullptr, STBI_rgb)).get());
    cubemap.assign(0, 0, 3, w, h, 1, GL_RGB, GL_FLOAT, res::stbi_data(stbi_loadf("../res/hdr/negy.hdr", &c, &c, nullptr, STBI_rgb)).get());
    cubemap.assign(0, 0, 4, w, h, 1, GL_RGB, GL_FLOAT, res::stbi_data(stbi_loadf("../res/hdr/posz.hdr", &c, &c, nullptr, STBI_rgb)).get());
    cubemap.assign(0, 0, 5, w, h, 1, GL_RGB, GL_FLOAT, res::stbi_data(stbi_loadf("../res/hdr/negz.hdr", &c, &c, nullptr, STBI_rgb)).get());
    cubemap.generate_mipmaps();

    gl::texture random_texture(GL_TEXTURE_2D, 512, 512, GL_RGBA16F, 1);
    std::vector<float> random_pixels(512 * 512 * 4);
    std::generate(random_pixels.begin(), random_pixels.end(), [gen = std::mt19937(), dist = std::uniform_real_distribution<float>(0.f, 1.f)]() mutable { return dist(gen); });
    random_texture.assign(GL_RGBA, GL_FLOAT, random_pixels.data());
    random_texture.generate_mipmaps();

    while (main_window->update())
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        draw_gui();
        controller.update(camera, *main_window, main_window->delta_time());
        
        main_renderer->bind();
        floor_pipeline->bind();
        floor_pipeline->at(GL_VERTEX_SHADER)->uniform<glm::mat4>("view_projection") = camera.projection() * camera.view();
        floor_pipeline->at(GL_FRAGMENT_SHADER)->uniform<glm::mat4>("inv_view") = inverse(camera.view());
        floor_pipeline->at(GL_FRAGMENT_SHADER)->uniform<float>("time") = float(glfwGetTime());
        floor_pipeline->at(GL_FRAGMENT_SHADER)->uniform<uint64_t>("cubemap") = sampler.sample(cubemap);
        floor_pipeline->at(GL_FRAGMENT_SHADER)->uniform<uint64_t>("random") = sampler.sample(random_texture);
        floor_pipeline->bind_attribute(0, vertex_buffer, 3, GL_FLOAT, offsetof(res::vertex, position));
        floor_pipeline->bind_attribute(1, vertex_buffer, 3, GL_FLOAT, offsetof(res::vertex, normal));
        floor_pipeline->bind_attribute(2, vertex_buffer, 2, GL_FLOAT, offsetof(res::vertex, uv));
        floor_pipeline->draw(GL_TRIANGLES, element_buffer, GL_UNSIGNED_INT);

        glDepthMask(false);
        cubemap_pipeline.bind();
        cubemap_pipeline[GL_VERTEX_SHADER]->uniform<glm::mat4>("cubemap_matrix") = inverse(camera.projection() * glm::mat4(glm::mat3(camera.view())));
        cubemap_pipeline[GL_FRAGMENT_SHADER]->uniform<uint64_t>("map") = sampler.sample(cubemap);
        cubemap_pipeline[GL_FRAGMENT_SHADER]->uniform<glm::vec4>("tint") = glm::vec4(0.9f, 0.96f, 1.f, 1.f);
        cubemap_pipeline.draw(GL_TRIANGLES, 3);
        glDepthMask(true);

        main_renderer->draw(main_window->delta_time());
    }
    return 0;
}

void draw_gui()
{
    ImGui::Begin("Settings");
    ImGui::Value("Delta-Time", static_cast<float>(main_window->delta_time()));
    if (ImGui::Button("Reload Pipeline"))
        floor_pipeline->reload();
    ImGui::End();
}