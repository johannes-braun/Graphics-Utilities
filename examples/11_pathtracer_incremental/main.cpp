#include <executable.hpp>
#include <random>

#include <gfx/graphics/vulkan/swapchain_vulkan.hpp>

void executable::init(gfx::context_options& opt)
{
    opt.window_title        = "[11] Incremental Pathtracer";
    opt.framebuffer_samples = gfx::sample_count::x1;
    opt.graphics_api        = gfx::gapi::vulkan;
}

struct helper_info
{
    glm::mat4 inverse_view_proj;
    float     time;
    gfx::u32  reset;
};

void executable::run()
{
    camera.transform_mode.position.z += 4.f;

    gfx::image accumulation_cache(gfx::img_type::image2d, gfx::rgba32f, gfx::extent(1280, 720), 1);
    gfx::image bounce_cache(gfx::img_type::image2d, gfx::rgba32f, gfx::extent(1280, 720), 1);
    gfx::image direction_cache(gfx::img_type::image2d, gfx::rgba32f, gfx::extent(1280, 720), 1);
    gfx::image origin_cache(gfx::img_type::image2d, gfx::rgba32f, gfx::extent(1280, 720), 1);
    gfx::image counter_cache(gfx::img_type::image2d, gfx::rg32u, gfx::extent(1280, 720), 1);

    const auto accumulation_cache_view = accumulation_cache.view(gfx::imgv_type::image2d);
    const auto bounce_cache_view       = bounce_cache.view(gfx::imgv_type::image2d);
    const auto direction_cache_view    = direction_cache.view(gfx::imgv_type::image2d);
    const auto origin_cache_view       = origin_cache.view(gfx::imgv_type::image2d);
    const auto counter_cache_view      = counter_cache.view(gfx::imgv_type::image2d);

    gfx::image&     cubemap      = res.cubemaps_hdr["arboretum/hdr"];
	const gfx::image_view cubemap_view = cubemap.view(gfx::imgv_type::image_cube);
	const gfx::sampler    sampler;

	gfx::binding_layout trace_layout;
	trace_layout.push(gfx::binding_type::uniform_buffer);
	trace_layout.push(gfx::binding_type::uniform_buffer);
	trace_layout.push(gfx::binding_type::storage_buffer);
	trace_layout.push(gfx::binding_type::storage_buffer);
	trace_layout.push(gfx::binding_type::storage_buffer);
	trace_layout.push(gfx::binding_type::sampled_image);
	trace_layout.push(gfx::binding_type::storage_image);
	trace_layout.push(gfx::binding_type::storage_image);
	trace_layout.push(gfx::binding_type::storage_image);
	trace_layout.push(gfx::binding_type::storage_image);
	trace_layout.push(gfx::binding_type::storage_image);

	gfx::pipe_state::binding_layouts trace_layouts;
	trace_layouts.layouts.push_back(&trace_layout);

	gfx::pipe_state trace_state;
	trace_state.state_bindings = &trace_layouts;
	trace_state.state_multisample = &msaa_state;

	gfx::graphics_pipeline trace_pipeline(trace_state, pass_layout, {
		gfx::shader(gfx::shader_type::vert, "postfx/screen.vert"),
		gfx::shader(gfx::shader_type::frag, "11_pathtracer_incremental/trace.frag")
	});

    gfx::scene_file& scene1      = res.scenes["wall.dae"];
	gfx::scene_file& scene2      = res.scenes["proto/serp.dae"];
	gfx::mesh3d      scene_mesh = scene1.mesh + scene2.mesh;
    scene_mesh.collapse();

    gfx::bvh<3> bvh(gfx::shape::triangle);
    bvh.sort(scene_mesh.indices.begin(), scene_mesh.indices.end(), [&](gfx::index32 i) { return scene_mesh.vertices[i].position; });
    const gfx::buffer<gfx::bvh<3>::node> model_bvh(gfx::buffer_usage::storage, bvh.nodes());
    const gfx::buffer<gfx::vertex3d>     vertices(gfx::buffer_usage::storage, scene_mesh.vertices);
    const gfx::buffer<gfx::index32>      indices(gfx::buffer_usage::storage, scene_mesh.indices);

    gfx::hbuffer<helper_info> helper_info_buffer(1);

	gfx::binding_set trace_set(trace_layout);
	trace_set.bind(0, *camera_buffer);
	trace_set.bind(1, helper_info_buffer);
	trace_set.bind(2, model_bvh);
	trace_set.bind(3, vertices);
	trace_set.bind(4, indices);
	trace_set.bind(5, cubemap_view, sampler);
	trace_set.bind(6, accumulation_cache_view);
	trace_set.bind(7, bounce_cache_view);
	trace_set.bind(8, direction_cache_view);
	trace_set.bind(9, origin_cache_view);
	trace_set.bind(10, counter_cache_view);

    std::mt19937                          gen;
    std::uniform_real_distribution<float> dist(0.f, 1.f);

    gfx::transform last_cam = camera.transform_mode;

    int   i    = 0;
    float time = 0;
    while (frame()) {
        ImGui::Begin("Settings");
        if (ImGui::Button("Reset Tracer")) i = 0;
        ImGui::Value("Iteration", i * 1);
        ImGui::Value("Min. Samples", (i * 1) / 8);
        ImGui::Value("Time", time);
        ImGui::End();

        if (last_cam != camera.transform_mode) {
            i        = 0;
            last_cam = camera.transform_mode;
        }

        const auto then = std::chrono::steady_clock::now();

        helper_info_buffer[0].inverse_view_proj =
            glm::mat4(glm::mat3(glm::inverse(camera_buffer->at(0).view))) * inverse(camera_buffer->at(0).projection);
        helper_info_buffer[0].time  = float(glfwGetTime()) * dist(gen);
        helper_info_buffer[0].reset = i++;

		current_command->begin_pass(*current_framebuffer);
        current_command->bind_pipeline(trace_pipeline, {&trace_set});
		current_command->draw(3);
		current_command->end_pass();
    }
}