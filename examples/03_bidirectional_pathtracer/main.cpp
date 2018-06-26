#include <gfx/gfx.hpp>
#include <memory>
#include <opengl/opengl.hpp>
#include <random>

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
    gfx::context_options options;
    options.window_title  = "[03] Bidirectional Pathtracer";
    options.window_height = 720;
    options.window_width  = 1280;
    options.debug         = true;
    auto context          = gfx::context::create(options);
    context->make_current();
    tracer = std::make_unique<gl::compute_pipeline>(
            std::make_shared<gl::shader>("03_bidirectional_pathtracer/trace.comp"));
    gfx::imgui imgui;

    glDebugMessageCallback(
            [](GLenum source, GLenum type, unsigned int id, GLenum severity, int length, const char* message, const void* userParam)
    { gfx::ilog << message;
    }, nullptr);

    gfx::scene_file        file("bunny.dae");
    gfx::scene_file::mesh& mesh = *(file.meshes.begin());

    gfx::bvh<3> gen_bvh(gfx::shape::triangle, gfx::bvh_mode::persistent_iterators);
    gen_bvh.sort(mesh.indices.begin(), mesh.indices.end(), [&](uint32_t index) {
        return mesh.vertices[index].position;
    });
    const std::vector<gl::byte>& packed = gen_bvh.pack(
            sizeof(gfx::vertex3d), offsetof(gfx::vertex3d, position), sizeof(uint32_t), 0);

    gl::buffer<gfx::vertex3d> vbo(mesh.vertices.begin(), mesh.vertices.end());
    gl::buffer<gfx::index32>  ibo(mesh.indices.begin(), mesh.indices.end());
    gl::buffer<gl::byte>      bvh(packed.begin(), packed.end());

    gl::query timer(GL_TIME_ELAPSED);

    auto render_texture = std::make_shared<gl::texture>(GL_TEXTURE_2D_ARRAY, 1280, 720, 1, GL_RGBA32F, 1);
    gl::image img(*render_texture, GL_RGBA32F, GL_READ_WRITE);

    gl::framebuffer framebuffer;
    framebuffer[GL_COLOR_ATTACHMENT0] = render_texture;

    gfx::camera                           camera;
    gfx::camera_controller                controller;
    std::mt19937                          gen;
    std::uniform_real_distribution<float> dist(0.f, 1.f);

    const auto[w, h, c] = gfx::image_file::info("hdri/hdr/posx.hdr");
    gl::texture cubemap(GL_TEXTURE_2D_ARRAY, w, h, 6, GL_R11F_G11F_B10F);
    cubemap.assign(0,
                   0,
                   0,
                   w,
                   h,
                   1,
                   GL_RGB,
                   GL_FLOAT,
                   gfx::image_file("hdri/hdr/posx.hdr", gfx::bits::b32, 3).bytes());
    cubemap.assign(0,
                   0,
                   1,
                   w,
                   h,
                   1,
                   GL_RGB,
                   GL_FLOAT,
                   gfx::image_file("hdri/hdr/negx.hdr", gfx::bits::b32, 3).bytes());
    cubemap.assign(0,
                   0,
                   2,
                   w,
                   h,
                   1,
                   GL_RGB,
                   GL_FLOAT,
                   gfx::image_file("hdri/hdr/posy.hdr", gfx::bits::b32, 3).bytes());
    cubemap.assign(0,
                   0,
                   3,
                   w,
                   h,
                   1,
                   GL_RGB,
                   GL_FLOAT,
                   gfx::image_file("hdri/hdr/negy.hdr", gfx::bits::b32, 3).bytes());
    cubemap.assign(0,
                   0,
                   4,
                   w,
                   h,
                   1,
                   GL_RGB,
                   GL_FLOAT,
                   gfx::image_file("hdri/hdr/posz.hdr", gfx::bits::b32, 3).bytes());
    cubemap.assign(0,
                   0,
                   5,
                   w,
                   h,
                   1,
                   GL_RGB,
                   GL_FLOAT,
                   gfx::image_file("hdri/hdr/negz.hdr", gfx::bits::b32, 3).bytes());
    cubemap.generate_mipmaps();

    mygl::texture tex;
    glGenTextures(1, &tex);
    glTextureView(tex, GL_TEXTURE_CUBE_MAP, cubemap, cubemap.internal_format(), 0, cubemap.levels(), 0, 6);

    const gl::sampler sampler;

    gl::buffer<tracer_data> data(1, GL_DYNAMIC_STORAGE_BIT);
    data[0].img       = img.handle();
    data[0].vbo       = vbo.handle();
    data[0].ibo       = ibo.handle();
    data[0].bvh       = bvh.handle();
    data[0].linespace = 0; // grid_line_space_datas.handle();
    data[0].cubemap   = sampler.sample(tex);
    data[0].frames    = 10;
    data.synchronize();

    while(context->run())
    {
        imgui.new_frame();
        controller.update(camera);
        const glm::mat4 camera_matrix =
                inverse(camera.projection.matrix() *
                        glm::mat4(glm::mat3(inverse(camera.transform.matrix()))));

        timer.start();
        data[0].camera_matrix   = camera_matrix;
        data[0].camera_position = glm::vec4(camera.transform.position, 1.f);
        data[0].seed            = dist(gen);
        data.synchronize();

        data.bind(GL_UNIFORM_BUFFER, 0);
        tracer->dispatch(1280, 720);

        framebuffer.blit(nullptr, GL_COLOR_BUFFER_BIT);
        timer.finish();

        ImGui::Begin("Settings");
        ImGui::Value("Time", float(timer.get<uint64_t>() / 1'000'000.0));

        static bool improve = false;
        ImGui::Checkbox("Improve", &improve);
        if(improve)
            data[0].frames++;
        ImGui::DragInt("Int. Samples", &data[0].frames, 0.1f, 1, 10000);

        if(ImGui::Button("Reload"))
            tracer->reload();
        if(ImGui::Button("Load"))
        {
            if(const auto item = gfx::file::open_dialog(
                       "Open Mesh",
                       "../",
                       {"*.dae", "*.fbx", "*.obj", "*.stl", "*.ply", "*.blend"},
                       "Meshes"))
            {
                gfx::scene_file            file(item.value());
                std::vector<uint32_t>      indices;
                std::vector<gfx::vertex3d> vertices;
                size_t                     begin = 0;
                int                        mid   = 0;
                for(const auto& mesh : file.meshes)
                {
                    vertices.reserve(vertices.size() + mesh.vertices.size());
                    const glm::mat4 model_matrix  = static_cast<glm::mat4>(mesh.transform);
                    const glm::mat4 normal_matrix = transpose(inverse(model_matrix));
                    for(auto vertex : mesh.vertices)
                    {
                        vertex.position = glm::vec3(model_matrix * glm::vec4(vertex.position, 1));
                        vertex.normal   = glm::vec3(normal_matrix * glm::vec4(vertex.normal, 0));
                        if(mid >= 1)
                            vertex.metadata_position = 1;
                        vertices.emplace_back(std::move(vertex));
                    }
                    ++mid;
                    indices.reserve(indices.size() + mesh.indices.size());
                    for(auto index : mesh.indices)
                        indices.emplace_back(uint32_t(index + begin));
                    begin += mesh.vertices.size();
                }

                gen_bvh.sort(indices.begin(), indices.end(), [&](uint32_t index) {
                    return vertices[index].position;
                });
                const std::vector<gl::byte>& packed =
                        gen_bvh.pack(sizeof(gfx::vertex3d),
                                     offsetof(gfx::vertex3d, position),
                                     sizeof(uint32_t),
                                     0);

                vbo         = gl::buffer<gfx::vertex3d>(vertices.begin(), vertices.end());
                ibo         = gl::buffer<gfx::index32>(indices.begin(), indices.end());
                bvh         = gl::buffer<gl::byte>(packed.begin(), packed.end());
                data[0].vbo = vbo.handle();
                data[0].ibo = ibo.handle();
                data[0].bvh = bvh.handle();
                data.synchronize();
            }
        }
        if(ImGui::Button("Save"))
        {
            if(const auto path =
                       gfx::file::save_dialog("Save Render", "../", {"*.png"}, "PNG Files"))
            {
                std::vector<float> data(1280 * 720 * 4);
                render_texture->get_data(
                        GL_RGBA, GL_FLOAT, 1280 * 720 * 4 * sizeof(float), data.data());

                std::vector<uint8_t> u8data(1280 * 720 * 4);
                for(int i = 0; i < 1280 * 720 * 4; ++i)
                    u8data[i] = uint8_t(std::clamp(data[i] * 255, 0.f, 255.f));

                gfx::image_file::save_png(path.value(), 1280, 720, 4, u8data.data());
            }
        }
        ImGui::End();
        imgui.render();
    }

    return 0;
}