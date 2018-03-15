#include <memory>
#include <random>

#include <io/window.hpp>
#include <io/camera.hpp>

#include <res/geometry.hpp>
#include <res/image.hpp>

#include <jpu/geometry.hpp>

#include <tinyfd/tinyfiledialogs.h>

#include <opengl/pipeline.hpp>
#include <opengl/texture.hpp>
#include <opengl/framebuffer.hpp>
#include <opengl/query.hpp>

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
    alignas(8) uintptr_t cubemap;
    alignas(4) float seed;
};

int main()
{
    window = std::make_unique<io::window>(io::api::opengl, 1280, 720, "Simple PT");
    tracer = std::make_unique<gl::compute_pipeline>(std::make_shared<gl::shader>("simple_pt/trace.comp"));
    
    res::geometry_file file = res::load_geometry("../res/cube.dae");
    res::mesh& mesh = file.meshes.get_by_index(0);
    jpu::bvh<3> mesh_bvh; mesh_bvh.assign_to(mesh.indices, mesh.vertices, &res::vertex::position, jpu::bvh_primitive_type::triangles);
    const std::vector<gl::byte> packed = mesh_bvh.pack();

    gl::buffer<res::vertex> vbo(mesh.vertices.begin(), mesh.vertices.end());
    gl::buffer<res::index32> ibo(mesh.indices.begin(), mesh.indices.end());
    gl::buffer<gl::byte> bvh(packed.begin(), packed.end());
    
    gl::query timer(GL_TIME_ELAPSED);

    auto render_texture = std::make_shared<gl::texture>(GL_TEXTURE_2D, 1280, 720, GL_RGBA32F, 1);
    gl::image img(*render_texture, GL_RGBA32F, GL_READ_WRITE);
    
    gl::framebuffer framebuffer;
    framebuffer[GL_COLOR_ATTACHMENT0] = render_texture;

    io::camera camera;
    io::default_cam_controller controller;
    std::mt19937 gen;
    std::uniform_real_distribution<float> dist(0.f, 1.f);

    const gl::sampler sampler;

    std::vector<res::image> cubemap_images;
    cubemap_images.emplace_back(load_image("../res/indoor/hdr/posx.hdr", res::image_type::f32, res::RGB));
    cubemap_images.emplace_back(load_image("../res/indoor/hdr/negx.hdr", res::image_type::f32, res::RGB));
    cubemap_images.emplace_back(load_image("../res/indoor/hdr/posy.hdr", res::image_type::f32, res::RGB));
    cubemap_images.emplace_back(load_image("../res/indoor/hdr/negy.hdr", res::image_type::f32, res::RGB));
    cubemap_images.emplace_back(load_image("../res/indoor/hdr/posz.hdr", res::image_type::f32, res::RGB));
    cubemap_images.emplace_back(load_image("../res/indoor/hdr/negz.hdr", res::image_type::f32, res::RGB));

    const int w = cubemap_images[0].width;
    const int h = cubemap_images[0].height;
    gl::texture cubemap(GL_TEXTURE_CUBE_MAP, w, h, GL_R11F_G11F_B10F);
    cubemap.assign(0, 0, 0, w, h, 1, GL_RGB, GL_FLOAT, cubemap_images[0].data.get());
    cubemap.assign(0, 0, 1, w, h, 1, GL_RGB, GL_FLOAT, cubemap_images[1].data.get());
    cubemap.assign(0, 0, 2, w, h, 1, GL_RGB, GL_FLOAT, cubemap_images[2].data.get());
    cubemap.assign(0, 0, 3, w, h, 1, GL_RGB, GL_FLOAT, cubemap_images[3].data.get());
    cubemap.assign(0, 0, 4, w, h, 1, GL_RGB, GL_FLOAT, cubemap_images[4].data.get());
    cubemap.assign(0, 0, 5, w, h, 1, GL_RGB, GL_FLOAT, cubemap_images[5].data.get());
    cubemap_images.clear();

    gl::buffer<tracer_data> data(1, GL_DYNAMIC_STORAGE_BIT);
    data[0].img = img.handle();
    data[0].vbo = vbo.handle();
    data[0].ibo = ibo.handle();
    data[0].bvh = bvh.handle();
    data[0].cubemap = sampler.sample(cubemap);
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
        tracer->dispatch(1280, 720);
        
        framebuffer.blit(nullptr, GL_COLOR_BUFFER_BIT);
        timer.finish();

        ImGui::Begin("Settings");
        ImGui::Value("Time", float(timer.get<uint64_t>() / 1'000'000.0));
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
                for (const auto& mesh : file.meshes)
                {
                    vertices.reserve(vertices.size() + mesh.vertices.size());
                    const glm::mat4 model_matrix = static_cast<glm::mat4>(mesh.transform);
                    const glm::mat4 normal_matrix = transpose(inverse(model_matrix));
                    for (auto vertex : mesh.vertices) {
                        vertex.position = glm::vec3(model_matrix * glm::vec4(vertex.position, 1));
                        vertex.normal = glm::vec3(normal_matrix * glm::vec4(vertex.normal, 0));
                        vertices.emplace_back(std::move(vertex));
                    }
                    indices.reserve(indices.size() + mesh.indices.size());
                    for (auto index : mesh.indices) indices.emplace_back(index + begin);
                    begin += mesh.vertices.size();
                }

                jpu::bvh<3> mesh_bvh; mesh_bvh.assign_to(indices, vertices, &res::vertex::position, jpu::bvh_primitive_type::triangles);
                const std::vector<gl::byte> packed = mesh_bvh.pack();
                vbo = gl::buffer<res::vertex>(vertices.begin(), vertices.end());
                ibo = gl::buffer<res::index32>(indices.begin(), indices.end());
                bvh = gl::buffer<gl::byte>(packed.begin(), packed.end());
                data[0].vbo = vbo.handle();
                data[0].ibo = ibo.handle();
                data[0].bvh = bvh.handle();
                data.synchronize();
            }
        }
        ImGui::End();
    }

    return 0;
}