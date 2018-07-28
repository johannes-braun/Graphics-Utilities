#include <gfx/gfx.hpp>
#include <gfx/res.hpp>
#include <random>
#include <runnable.hpp>

void runnable::init(gfx::context_options& opt)
{
    opt.window_title        = "[11] Incremental Pathtracer";
    opt.framebuffer_samples = 1;
    opt.graphics_api        = gfx::gapi::opengl;
}

struct helper_info
{
    glm::mat4 inverse_view_proj;
    float     time;
    gfx::u32  reset;
};

void runnable::run()
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

    gfx::image&     cubemap      = res.cubemaps_hdr["ninomaru_teien/hdr"];
	const gfx::image_view cubemap_view = cubemap.view(gfx::imgv_type::image_cube);
	const gfx::sampler    sampler;

    gfx::compute_pipeline trace_pipeline(gfx::shader(gfx::shader_format::text, "11_pathtracer_incremental/trace.comp"));

    gfx::scene_file& scene1      = res.scenes["corners.dae"];
	gfx::scene_file& scene2      = res.scenes["cubus.dae"];
	gfx::scene_file& scene3      = res.scenes["isosphere.dae"];
    gfx::mesh3d      scene_mesh = scene1.mesh + scene2.mesh + scene3.mesh;
    scene_mesh.collapse();

    gfx::bvh<3> bvh(gfx::shape::triangle);
    bvh.sort(scene_mesh.indices.begin(), scene_mesh.indices.end(), [&](gfx::index32 i) { return scene_mesh.vertices[i].position; });
    const gfx::buffer<gfx::bvh<3>::node> model_bvh(gfx::buffer_usage::storage, bvh.nodes());
    const gfx::buffer<gfx::vertex3d>     vertices(gfx::buffer_usage::storage, scene_mesh.vertices);
    const gfx::buffer<gfx::index32>      indices(gfx::buffer_usage::storage, scene_mesh.indices);

    gfx::hbuffer<helper_info> helper_info_buffer(1);

    std::vector<gfx::descriptor_set> trace_descriptors(context->swapchain()->image_views().size());
    for (int i = 0; i < trace_descriptors.size(); ++i) {
        trace_descriptors[i].set(gfx::descriptor_type::uniform_buffer, 0, *camera_buffer);
        trace_descriptors[i].set(gfx::descriptor_type::uniform_buffer, 1, helper_info_buffer);

        trace_descriptors[i].set(gfx::descriptor_type::storage_buffer, 0, model_bvh);
        trace_descriptors[i].set(gfx::descriptor_type::storage_buffer, 1, vertices);
        trace_descriptors[i].set(gfx::descriptor_type::storage_buffer, 2, indices);

        trace_descriptors[i].set(gfx::descriptor_type::sampled_texture, 0, cubemap_view, sampler);

        trace_descriptors[i].set(gfx::descriptor_type::storage_image, 0, accumulation_cache_view);
        trace_descriptors[i].set(gfx::descriptor_type::storage_image, 1, bounce_cache_view);
        trace_descriptors[i].set(gfx::descriptor_type::storage_image, 2, direction_cache_view);
        trace_descriptors[i].set(gfx::descriptor_type::storage_image, 3, origin_cache_view);
        trace_descriptors[i].set(gfx::descriptor_type::storage_image, 4, counter_cache_view);
        trace_descriptors[i].set(gfx::descriptor_type::storage_image, 5, context->swapchain()->image_views()[i]);
    }
    gfx::commands cmd;
    gfx::fence    update_fence;

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
        cmd.reset();
        cmd.bind_pipeline(trace_pipeline);
        cmd.bind_descriptors({&trace_descriptors[context->swapchain()->current_image()], 1});
        cmd.dispatch_compute((1280 + 15) / 16, (720 + 15) / 16);
        cmd.execute(update_fence);
        update_fence.wait();
        time = (std::chrono::steady_clock::now() - then).count() / 1'000'000.f;
    }
}