#include <memory>
#include <random>

#include <tinyfd/tinyfiledialogs.h>

#include <opengl/pipeline.hpp>
#include <opengl/texture.hpp>
#include <opengl/framebuffer.hpp>
#include <opengl/query.hpp>

#include <gfx/data/bvh.hpp>
#include <gfx/data/grid_line_space.hpp>
#include <gfx/data/gpu_data.hpp>

#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include <gfx/file.hpp>
#include <gfx/geometry.hpp>
#include <gfx/window.hpp>
#include <gfx/imgui.hpp>
#include <gfx/camera.hpp>


std::shared_ptr<gfx::window> window;
std::unique_ptr<gl::compute_pipeline> tracer;

struct tracer_data
{
    alignas(16) glm::mat4 camera_matrix;
    alignas(16) glm::vec4 camera_position;

    alignas(16) uintptr_t img;
    alignas(16) uintptr_t bvh;
    alignas(8) uintptr_t vbo;
    alignas(8) uintptr_t ibo;
    alignas(8) uintptr_t linespace;
    alignas(8) uintptr_t cubemap;
    alignas(4) float seed;
    alignas(4) int frames;
};

int main()
{
    gl::shader::set_include_directories(std::vector<gfx::files::path>{ "../shd", SOURCE_DIRECTORY "/global/shd" });

    window = std::make_shared<gfx::window>(gfx::apis::opengl::name, "Simple PT", 800, 600);
    tracer = std::make_unique<gl::compute_pipeline>(std::make_shared<gl::shader>("trace.comp"));
    gfx::imgui imgui(window);

    gfx::scene_file file("sphaear.dae");
    gfx::scene_file::mesh& mesh = *(file.meshes.begin());

    gfx::bvh<3> gen_bvh(gfx::shape::triangle, gfx::bvh_mode::persistent_iterators);
    gen_bvh.sort(mesh.indices.begin(), mesh.indices.end(), [&](uint32_t index) { return mesh.vertices[index].position; });
    const std::vector<gl::byte>& packed = gen_bvh.pack(sizeof(gfx::vertex3d), offsetof(gfx::vertex3d, position), sizeof(uint32_t), 0);

    gl::buffer<gfx::vertex3d> vbo(mesh.vertices.begin(), mesh.vertices.end());
    gl::buffer<gfx::index32> ibo(mesh.indices.begin(), mesh.indices.end());
    gl::buffer<gl::byte> bvh(packed.begin(), packed.end());

    gl::query timer(GL_TIME_ELAPSED);

    auto render_texture = std::make_shared<gl::texture>(GL_TEXTURE_2D, 800, 600, GL_RGBA32F, 1);
    gl::image img(*render_texture, GL_RGBA32F, GL_READ_WRITE);

    gl::framebuffer framebuffer;
    framebuffer[GL_COLOR_ATTACHMENT0] = render_texture;

    gfx::camera camera;
    gfx::camera_controller controller(window);
    std::mt19937 gen;
    std::uniform_real_distribution<float> dist(0.f, 1.f);

    const auto [w, h, c] =  gfx::image_file::info("indoor/posx.hdr");
    gl::texture cubemap(GL_TEXTURE_CUBE_MAP, w, h, GL_R11F_G11F_B10F);
    cubemap.assign(0, 0, 0, w, h, 1, GL_RGB, GL_FLOAT, gfx::image_file("indoor/posx.hdr", gfx::bits::b32, 3).bytes());
    cubemap.assign(0, 0, 1, w, h, 1, GL_RGB, GL_FLOAT, gfx::image_file("indoor/negx.hdr", gfx::bits::b32, 3).bytes());
    cubemap.assign(0, 0, 2, w, h, 1, GL_RGB, GL_FLOAT, gfx::image_file("indoor/posy.hdr", gfx::bits::b32, 3).bytes());
    cubemap.assign(0, 0, 3, w, h, 1, GL_RGB, GL_FLOAT, gfx::image_file("indoor/negy.hdr", gfx::bits::b32, 3).bytes());
    cubemap.assign(0, 0, 4, w, h, 1, GL_RGB, GL_FLOAT, gfx::image_file("indoor/posz.hdr", gfx::bits::b32, 3).bytes());
    cubemap.assign(0, 0, 5, w, h, 1, GL_RGB, GL_FLOAT, gfx::image_file("indoor/negz.hdr", gfx::bits::b32, 3).bytes());
    cubemap.generate_mipmaps();
    const gl::sampler sampler;

    gl::buffer<tracer_data> data(1, GL_DYNAMIC_STORAGE_BIT);
    data[0].img = img.handle();
    data[0].vbo = vbo.handle();
    data[0].ibo = ibo.handle();
    data[0].bvh = bvh.handle();
    data[0].linespace = 0;//grid_line_space_datas.handle();
    data[0].cubemap = sampler.sample(cubemap);
    data[0].frames = 10;
    data.synchronize();

    while (window->update())
    {
        imgui.new_frame();
        controller.update(camera);
        const glm::mat4 camera_matrix = inverse(camera.projection.matrix() * glm::mat4(glm::mat3(inverse(camera.transform.matrix()))));

        timer.start();
        data[0].camera_matrix = camera_matrix;
        data[0].camera_position = glm::vec4(camera.transform.position, 1.f);
        data[0].seed = dist(gen);
        data.synchronize();

        data.bind(GL_UNIFORM_BUFFER, 0);
        tracer->dispatch(800, 600);

        framebuffer.blit(nullptr, GL_COLOR_BUFFER_BIT);
        timer.finish();

        ImGui::Begin("Settings");
        ImGui::Value("Time", float(timer.get<uint64_t>() / 1'000'000.0));

        static bool improve = false;
        ImGui::Checkbox("Improve", &improve);
        if (improve) data[0].frames++;
        ImGui::DragInt("Int. Samples", &data[0].frames, 0.1f, 1, 10000);

        if (ImGui::Button("Reload")) tracer->reload();
        if (ImGui::Button("Load"))
        {
            constexpr const char *fs[6] = { "*.dae", "*.fbx", "*.obj", "*.stl", "*.ply", "*.blend" };
            if (const char* item = tinyfd_openFileDialog("Open Mesh", "../", 6, fs, "mesh", false))
            {
                gfx::scene_file file(item);
                std::vector<uint32_t> indices;
                std::vector<gfx::vertex3d> vertices;
                size_t begin = 0;
                int mid = 0;
                for (const auto& mesh : file.meshes)
                {
                    vertices.reserve(vertices.size() + mesh.vertices.size());
                    const glm::mat4 model_matrix = static_cast<glm::mat4>(mesh.transform);
                    const glm::mat4 normal_matrix = transpose(inverse(model_matrix));
                    for (auto vertex : mesh.vertices) {
                        vertex.position = glm::vec3(model_matrix * glm::vec4(vertex.position, 1));
                        vertex.normal = glm::vec3(normal_matrix * glm::vec4(vertex.normal, 0));
                        if (mid >= 1)
                            vertex.metadata_position = 1;
                        vertices.emplace_back(std::move(vertex));
                    }
                    ++mid;
                    indices.reserve(indices.size() + mesh.indices.size());
                    for (auto index : mesh.indices) indices.emplace_back(uint32_t(index + begin));
                    begin += mesh.vertices.size();
                }

                gen_bvh.sort(indices.begin(), indices.end(), [&](uint32_t index) { return vertices[index].position; });
                const std::vector<gl::byte>& packed = gen_bvh.pack(sizeof(gfx::vertex3d), offsetof(gfx::vertex3d, position), sizeof(uint32_t), 0);

                vbo = gl::buffer<gfx::vertex3d>(vertices.begin(), vertices.end());
                ibo = gl::buffer<gfx::index32>(indices.begin(), indices.end());
                bvh = gl::buffer<gl::byte>(packed.begin(), packed.end());
                data[0].vbo = vbo.handle();
                data[0].ibo = ibo.handle();
                data[0].bvh = bvh.handle();
                data.synchronize();
            }
        }
        if (ImGui::Button("Save"))
        {
            if (const char* path = tinyfd_saveFileDialog("Save Render", "../", 0, nullptr, "All Files"))
            {
                std::vector<float> data(800 * 600 * 4);
                render_texture->get_data(GL_RGBA, GL_FLOAT, 800 * 600 * 4 * sizeof(float), data.data());

                std::vector<uint8_t> u8data(800 * 600 * 4);
                for (int i = 0; i < 800 * 600 * 4; ++i)
                    u8data[i] = uint8_t(std::clamp(data[i] * 255, 0.f, 255.f));

                stbi_write_png(path, 800, 600, 4, u8data.data(), 0);
            }
        }
        ImGui::End();
        imgui.render();
    }

    return 0;
}