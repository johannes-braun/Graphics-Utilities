#define GFX_EXPOSE_APIS
#include <gfx/gfx.hpp>
#include <memory>
#include <opengl/opengl.hpp>
#include <random>

std::unique_ptr<gl::compute_pipeline> tracer;

struct tracer_data
{
    alignas(16) glm::mat4 camera_matrix;
    alignas(16) glm::vec4 camera_position;

    alignas(16) uintptr_t bvh;
    alignas(8) uintptr_t vbo;
    alignas(8) uintptr_t ibo;
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
    tracer = std::make_unique<gl::compute_pipeline>(std::make_shared<gl::shader>("03_bidirectional_pathtracer/trace.comp"));
    gfx::imgui imgui;

    gfx::scene_file        file("bunny.dae");
    gfx::scene_file::mesh& mesh = *(file.meshes.begin());

    gfx::host_buffer<gfx::vertex3d> vertices = mesh.vertices;
    gfx::host_buffer<gfx::index32>  indices  = mesh.indices;

    gfx::bvh<3> gen_bvh(gfx::shape::triangle, gfx::bvh_mode::persistent_iterators);
    gen_bvh.sort(indices.begin(), indices.end(), [&](uint32_t index) { return vertices[index].position; });
    gfx::host_buffer<uint8_t> packed_bvh = gen_bvh.pack(sizeof(gfx::vertex3d), offsetof(gfx::vertex3d, position), sizeof(uint32_t), 0);

    gfx::device_buffer<gfx::vertex3d> vbo(gfx::buffer_usage::storage, vertices);
    gfx::device_buffer<gfx::index32>  ibo(gfx::buffer_usage::storage, indices);
    gfx::device_buffer<uint8_t>       bvh(gfx::buffer_usage::storage, packed_bvh);

    gl::query timer(GL_TIME_ELAPSED);

    gfx::device_image render_target(gfx::img_type::image2d, gfx::rgba32f, {1280, 720}, 1);
    gl::framebuffer   framebuffer;
    glNamedFramebufferTextureLayer(framebuffer, GL_COLOR_ATTACHMENT0, handle_cast<mygl::texture>(render_target), 0, 0);

    gfx::camera                           camera;
    gfx::camera_controller                controller;
    std::mt19937                          gen;
    std::uniform_real_distribution<float> dist(0.f, 1.f);

    const auto        cubemap_format = gfx::r11g11b10f;
    const auto        info           = gfx::image_file::info("hdri/hdr/posx.hdr");
    gfx::device_image cubemap_texture(gfx::img_type::image2d, cubemap_format, gfx::extent(info.width, info.height, 6), 1);
    cubemap_texture.layer(0) << gfx::host_image(cubemap_format, "hdri/hdr/posx.hdr");
    cubemap_texture.layer(1) << gfx::host_image(cubemap_format, "hdri/hdr/negx.hdr");
    cubemap_texture.layer(2) << gfx::host_image(cubemap_format, "hdri/hdr/posy.hdr");
    cubemap_texture.layer(3) << gfx::host_image(cubemap_format, "hdri/hdr/negy.hdr");
    cubemap_texture.layer(4) << gfx::host_image(cubemap_format, "hdri/hdr/posz.hdr");
    cubemap_texture.layer(5) << gfx::host_image(cubemap_format, "hdri/hdr/negz.hdr");
    gfx::image_view cubemap_view(gfx::imgv_type::image_cube, cubemap_format, cubemap_texture, 0, cubemap_texture.levels(), 0, 6);

    gfx::sampler sampler;

    gfx::host_buffer<tracer_data> data_buffer(1);

    const auto get_handle = [](const auto& buf) -> uint64_t {
        uint64_t hnd;
        glGetNamedBufferParameterui64vNV(handle_cast<mygl::buffer>(buf), GL_BUFFER_GPU_ADDRESS_NV, &hnd);
        glMakeNamedBufferResidentNV(handle_cast<mygl::buffer>(buf), GL_READ_WRITE);
        return hnd;
    };

    data_buffer[0].vbo    = get_handle(vbo);
    data_buffer[0].ibo    = get_handle(ibo);
    data_buffer[0].bvh    = get_handle(bvh);
    data_buffer[0].frames = 10;

    while(context->run())
    {
        imgui.new_frame();

		glClear(GL_COLOR_BUFFER_BIT);
		
        controller.update(camera);

        timer.start();
        data_buffer[0].camera_matrix =
                inverse(camera.projection_mode.matrix() * glm::mat4(glm::mat3(inverse(camera.transform_mode.matrix()))));
        data_buffer[0].camera_position = glm::vec4(camera.transform_mode.position, 1.f);
        data_buffer[0].seed            = dist(gen);
        glBindSampler(0, handle_cast<mygl::sampler>(sampler));
        glBindTextureUnit(0, handle_cast<mygl::texture>(cubemap_view));
        glBindImageTexture(0, handle_cast<mygl::texture>(render_target), 0, true, 0, GL_READ_WRITE, GL_RGBA32F);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, handle_cast<mygl::buffer>(data_buffer));
        tracer->dispatch(1280, 720);

        framebuffer.blit(nullptr, 0, 0, 1280, 720, GL_COLOR_BUFFER_BIT);
        timer.finish();

        ImGui::Begin("Settings");
        ImGui::Value("Time", float(timer.get<uint64_t>() / 1'000'000.0));

        static bool improve = false;
        ImGui::Checkbox("Improve", &improve);
        if(improve)
            data_buffer[0].frames++;
        ImGui::DragInt("Int. Samples", &data_buffer[0].frames, 0.1f, 1, 10000);

        if(ImGui::Button("Reload"))
            tracer->reload();
        if(ImGui::Button("Load"))
        {
            if(const auto item =
                       gfx::file::open_dialog("Open Mesh", "../", {"*.dae", "*.fbx", "*.obj", "*.stl", "*.ply", "*.blend"}, "Meshes"))
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

                gfx::host_buffer<gfx::vertex3d> xvertices = vertices;
                gfx::host_buffer<gfx::index32>  xindices  = indices;

                gen_bvh.sort(xindices.begin(), xindices.end(), [&](uint32_t index) { return xvertices[index].position; });
                gfx::host_buffer<uint8_t> xpacked_bvh =
                        gen_bvh.pack(sizeof(gfx::vertex3d), offsetof(gfx::vertex3d, position), sizeof(uint32_t), 0);

                vbo = gfx::device_buffer<gfx::vertex3d>(gfx::buffer_usage::storage, xvertices);
                ibo = gfx::device_buffer<gfx::index32>(gfx::buffer_usage::storage, xindices);
                bvh = gfx::device_buffer<uint8_t>(gfx::buffer_usage::storage, xpacked_bvh);

                data_buffer[0].vbo = get_handle(vbo);
                data_buffer[0].ibo = get_handle(ibo);
                data_buffer[0].bvh = get_handle(bvh);
            }
        }
        if(ImGui::Button("Save"))
        {
            if(const auto path = gfx::file::save_dialog("Save Render", "../", {"*.png"}, "PNG Files"))
            {
                gfx::host_image render_data(render_target.pixel_format(), render_target.extents());
                render_target.level(0) >> render_data;
                render_data.flip_vertically();

                gfx::image_file::save_png(path.value(),
                                          1280,
                                          720,
                                          3,
                                          reinterpret_cast<const uint8_t*>(&(render_data.converted(gfx::rgb8unorm).storage()[0])));
            }
        }
        ImGui::End();
        imgui.render();
    }

    return 0;
}