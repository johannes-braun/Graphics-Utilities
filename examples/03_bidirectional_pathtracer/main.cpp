#define GFX_EXPOSE_APIS
#include <gfx/gfx.hpp>
#include <memory>
#include <opengl/opengl.hpp>
#include <random>
#include "gfx/res.hpp"


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
	gfx::ressources res;

    gfx::context_options options;
    options.window_title  = "[03] Bidirectional Pathtracer";
    options.window_height = 720;
    options.window_width  = 1280;
	options.framebuffer_samples = 1;
    options.debug         = true;
    auto context          = gfx::context::create(options);
    context->make_current();

	gfx::compute_pipeline trace_pipeline(gfx::shader(gfx::shader_format::text, "03_bidirectional_pathtracer/trace.comp"));

    gfx::imgui imgui;

    gfx::scene_file&        file = res.scenes["bunny.dae"];
	gfx::mesh3d mesh = file.mesh;
	mesh.collapse();

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
    gfx::image_view render_target_view(gfx::imgv_type::image2d, render_target);

    gfx::camera                           camera;
    gfx::camera_controller                controller;
    std::mt19937                          gen;
    std::uniform_real_distribution<float> dist(0.f, 1.f);

	gfx::device_image& cubemap_texture = res.cubemaps_hdr["hdri/hdr"];
    gfx::image_view cubemap_view(gfx::imgv_type::image_cube, cubemap_texture);
    gfx::sampler sampler;

    gfx::host_buffer<tracer_data> data_buffer(1);

    const auto get_handle = [](const auto& buf) -> uint64_t {
        uint64_t hnd;
        glGetNamedBufferParameterui64vNV(gfx::handle_cast<mygl::buffer>(buf), GL_BUFFER_GPU_ADDRESS_NV, &hnd);
        glMakeNamedBufferResidentNV(gfx::handle_cast<mygl::buffer>(buf), GL_READ_WRITE);
        return hnd;
    };

    data_buffer[0].vbo    = get_handle(vbo);
    data_buffer[0].ibo    = get_handle(ibo);
    data_buffer[0].bvh    = get_handle(bvh);
    data_buffer[0].frames = 10;

	gfx::framebuffer fb1(1280, 720);
	fb1.attach(gfx::attachment::color, 0, render_target_view);

	std::vector<gfx::framebuffer> fbos;
    for (const auto& i : context->swapchain()->image_views()) {
		fbos.emplace_back(1280, 720);
		fbos.back().attach(gfx::attachment::color, 0, i);
    }

	gfx::descriptor_set trace_descriptor;
	trace_descriptor.set(gfx::descriptor_type::sampled_texture, 0, cubemap_view, sampler);
	trace_descriptor.set(gfx::descriptor_type::storage_image, 0, render_target_view);
	trace_descriptor.set(gfx::descriptor_type::uniform_buffer, 0, data_buffer);

	gfx::commands cmd;

    while(context->run())
    {
        imgui.new_frame();

        controller.update(camera);

        data_buffer[0].camera_matrix =
                inverse(camera.projection_mode.matrix() * glm::mat4(glm::mat3(inverse(camera.transform_mode.matrix()))));
        data_buffer[0].camera_position = glm::vec4(camera.transform_mode.position, 1.f);
        data_buffer[0].seed            = dist(gen);

		timer.start();
		cmd.reset();
		cmd.bind_pipeline(trace_pipeline);
		cmd.bind_descriptors({ &trace_descriptor, 1 });
		cmd.dispatch_compute((1280 + 15) / 16, (720 + 7) / 8);
		cmd.execute();
        timer.finish();

		const auto fbh2 = gfx::handle_cast<mygl::framebuffer>(fbos[context->swapchain()->current_image()]);
		const auto fbh1 = gfx::handle_cast<mygl::framebuffer>(fb1);
		glNamedFramebufferReadBuffer(fbh1, GL_COLOR_ATTACHMENT0);
		glNamedFramebufferDrawBuffer(fbh2, GL_COLOR_ATTACHMENT0);
		glBlitNamedFramebuffer(fbh1, fbh2, 0, 0, 1280, 720, 0, 0, 1280, 720, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, fbh2);

        ImGui::Begin("Settings");
        ImGui::Value("Time", float(timer.get<uint64_t>() / 1'000'000.0));

        static bool improve = false;
        ImGui::Checkbox("Improve", &improve);
        if(improve)
            data_buffer[0].frames++;
        ImGui::DragInt("Int. Samples", &data_buffer[0].frames, 0.1f, 1, 10000);

        if(ImGui::Button("Load"))
        {
            if(const auto item =
                       gfx::file::open_dialog("Open Mesh", "../", {"*.dae", "*.fbx", "*.obj", "*.stl", "*.ply", "*.blend"}, "Meshes"))
            {
				gfx::scene_file&        file = res.scenes[item.value().path.string()];
				file.mesh.collapse();

                gen_bvh.sort(file.mesh.indices.begin(), file.mesh.indices.end(), [&](uint32_t index) { return file.mesh.vertices[index].position; });
                gfx::host_buffer<uint8_t> xpacked_bvh =
                        gen_bvh.pack(sizeof(gfx::vertex3d), offsetof(gfx::vertex3d, position), sizeof(uint32_t), 0);

                vbo = gfx::device_buffer<gfx::vertex3d>(gfx::buffer_usage::storage, file.mesh.vertices);
                ibo = gfx::device_buffer<gfx::index32>(gfx::buffer_usage::storage, file.mesh.indices);
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