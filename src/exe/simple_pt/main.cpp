#include <memory>
#include <random>

#include <io/window.hpp>
#include <io/camera.hpp>

#include <res/geometry.hpp>
#include <res/image.hpp>

#include <tinyfd/tinyfiledialogs.h>

#include <opengl/pipeline.hpp>
#include <opengl/texture.hpp>
#include <opengl/framebuffer.hpp>
#include <opengl/query.hpp>

#include <framework/data/bvh.hpp>
#include <framework/data/linespace.hpp>

std::unique_ptr<io::window> window;
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
    window = std::make_unique<io::window>(io::api::opengl, 800, 600, "Simple PT");
    tracer = std::make_unique<gl::compute_pipeline>(std::make_shared<gl::shader>("simple_pt/trace.comp"));

    res::geometry_file file = res::load_geometry("../res/cube.dae");
    res::mesh& mesh = file.meshes.get_by_index(0);

    gfx::bvh<3> gen_bvh(gfx::shape::triangle, gfx::bvh_mode::persistent_iterators);
    gen_bvh.sort(mesh.indices.begin(), mesh.indices.end(), [&](uint32_t index) {
        return mesh.vertices[index].position;
    });
    const std::vector<gl::byte>& packed = gen_bvh.pack(sizeof(res::vertex), offsetof(res::vertex, position), sizeof(uint32_t), 0);

    auto bound = gen_bvh.get_bounds();

    int gx = 2, gy = gx, gz = gx;
    const glm::vec4 qsize = bound.size() / glm::vec4{ gx, gy, gz, 1 };
    std::vector<gfx::line_space> line_spaces;
    gl::buffer<gfx::line_space::data_type> line_space_datas(GL_DYNAMIC_STORAGE_BIT);
    line_spaces.reserve(gx * gy * gz);
    line_space_datas.resize(gx * gy * gz);
    for (int z = 0; z < gz; ++z)
    {
        for (int y = 0; y < gy; ++y)
        {
            for (int x = 0; x < gx; ++x)
            {
                gfx::line_space_bounds lsb;
                lsb.min = bound.min + glm::vec4(x, y, z, 1) * qsize;
                lsb.max = lsb.min + qsize;
                line_spaces.emplace_back(2, 2, 2).build(gen_bvh, lsb);
                line_space_datas[z * gy * gx + y * gx + x] = (line_spaces.back().get_data());
            }
        }
    }
    line_space_datas.synchronize();
    struct grid_ls
    {
        gfx::line_space_bounds bounds;
        glm::ivec4 resolution;
        uint64_t line_spaces;
    };

    gl::buffer<grid_ls> grid_line_space(1, GL_DYNAMIC_STORAGE_BIT);
    grid_line_space[0].bounds = bound;
    grid_line_space[0].resolution = { gx, gy, gz, 1 };
    grid_line_space[0].line_spaces = line_space_datas.handle();
    grid_line_space.synchronize();


    gl::buffer<res::vertex> vbo(mesh.vertices.begin(), mesh.vertices.end());
    gl::buffer<res::index32> ibo(mesh.indices.begin(), mesh.indices.end());
    gl::buffer<gl::byte> bvh(packed.begin(), packed.end());

    gl::query timer(GL_TIME_ELAPSED);

    auto render_texture = std::make_shared<gl::texture>(GL_TEXTURE_2D, 800, 600, GL_RGBA32F, 1);
    gl::image img(*render_texture, GL_RGBA32F, GL_READ_WRITE);

    gl::framebuffer framebuffer;
    framebuffer[GL_COLOR_ATTACHMENT0] = render_texture;

    io::camera camera;
    io::default_cam_controller controller;
    std::mt19937 gen;
    std::uniform_real_distribution<float> dist(0.f, 1.f);

    const auto [w, h, c] = res::load_image_info("../res/indoor/hdr/posx.hdr");
    gl::texture cubemap(GL_TEXTURE_CUBE_MAP, w, h, GL_R11F_G11F_B10F);
    cubemap.assign(0, 0, 0, w, h, 1, GL_RGB, GL_FLOAT, load_image("../res/indoor/hdr/posx.hdr", res::image_type::f32, res::RGB).data.get());
    cubemap.assign(0, 0, 1, w, h, 1, GL_RGB, GL_FLOAT, load_image("../res/indoor/hdr/negx.hdr", res::image_type::f32, res::RGB).data.get());
    cubemap.assign(0, 0, 2, w, h, 1, GL_RGB, GL_FLOAT, load_image("../res/indoor/hdr/posy.hdr", res::image_type::f32, res::RGB).data.get());
    cubemap.assign(0, 0, 3, w, h, 1, GL_RGB, GL_FLOAT, load_image("../res/indoor/hdr/negy.hdr", res::image_type::f32, res::RGB).data.get());
    cubemap.assign(0, 0, 4, w, h, 1, GL_RGB, GL_FLOAT, load_image("../res/indoor/hdr/posz.hdr", res::image_type::f32, res::RGB).data.get());
    cubemap.assign(0, 0, 5, w, h, 1, GL_RGB, GL_FLOAT, load_image("../res/indoor/hdr/negz.hdr", res::image_type::f32, res::RGB).data.get());
    const gl::sampler sampler;

    gl::buffer<tracer_data> data(1, GL_DYNAMIC_STORAGE_BIT);
    data[0].img = img.handle();
    data[0].vbo = vbo.handle();
    data[0].ibo = ibo.handle();
    data[0].bvh = bvh.handle();
    data[0].linespace = grid_line_space.handle();
    data[0].cubemap = sampler.sample(cubemap);
    data[0].frames = 10;
    data.synchronize();

    while (window->update())
    {
        controller.update(camera, *window, window->delta_time());
        const glm::mat4 camera_matrix = inverse(camera.projection() * glm::mat4(glm::mat3(camera.view())));

        timer.start();
        data[0].camera_matrix = camera_matrix;
        data[0].camera_position = glm::vec4(camera.transform.position, 1.f);
        data[0].seed = dist(gen);
        data.synchronize();

        tracer->bind_uniform_buffer(0, data);
        tracer->dispatch(800, 600);

        render_texture->at(10, 10, 0) = glm::vec4(1, 0, 0, 1);

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
            if (const char* item = tinyfd_openFileDialog("Open Mesh", "../res/", 6, fs, "mesh", false))
            {
                res::geometry_file file = res::load_geometry(item);
                std::vector<uint32_t> indices;
                std::vector<res::vertex> vertices;
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
                            vertex.meta = 1;
                        vertices.emplace_back(std::move(vertex));
                    }
                    ++mid;
                    indices.reserve(indices.size() + mesh.indices.size());
                    for (auto index : mesh.indices) indices.emplace_back(uint32_t(index + begin));
                    begin += mesh.vertices.size();
                }

                gen_bvh.sort(indices.begin(), indices.end(), [&](uint32_t index) { return vertices[index].position; });
                const std::vector<gl::byte>& packed = gen_bvh.pack(sizeof(res::vertex), offsetof(res::vertex, position), sizeof(uint32_t), 0);

                vbo = gl::buffer<res::vertex>(vertices.begin(), vertices.end());
                ibo = gl::buffer<res::index32>(indices.begin(), indices.end());
                bvh = gl::buffer<gl::byte>(packed.begin(), packed.end());
                data[0].vbo = vbo.handle();
                data[0].ibo = ibo.handle();
                data[0].bvh = bvh.handle();
                data.synchronize();
            }
        }
        if (ImGui::Button("Save"))
        {
            if (const char* path = tinyfd_saveFileDialog("Save Render", "../res", 0, nullptr, "All Files"))
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
    }

    return 0;
}