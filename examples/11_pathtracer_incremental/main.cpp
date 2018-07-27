#include <gfx/gfx.hpp>
#include <random>
#include <runnable.hpp>
#include <gfx/res.hpp>

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
	gfx::ressources res;

    camera.transform_mode.position.z += 4.f;

    gfx::image accumulation_cache(gfx::img_type::image2d, gfx::rgba32f, gfx::extent(1280, 720), 1);
    gfx::image bounce_cache(gfx::img_type::image2d, gfx::rgba32f, gfx::extent(1280, 720), 1);
    gfx::image direction_cache(gfx::img_type::image2d, gfx::rgba32f, gfx::extent(1280, 720), 1);
    gfx::image origin_cache(gfx::img_type::image2d, gfx::rgba32f, gfx::extent(1280, 720), 1);
	gfx::image counter_cache(gfx::img_type::image2d, gfx::rg32u, gfx::extent(1280, 720), 1);

    gfx::image_view accumulation_cache_view = accumulation_cache.view(gfx::imgv_type::image2d);
    gfx::image_view bounce_cache_view       = bounce_cache.view(gfx::imgv_type::image2d);
    gfx::image_view direction_cache_view    = direction_cache.view(gfx::imgv_type::image2d);
    gfx::image_view origin_cache_view       = origin_cache.view(gfx::imgv_type::image2d);
    gfx::image_view counter_cache_view      = counter_cache.view(gfx::imgv_type::image2d);

	gfx::image& cubemap = res.cubemaps_hdr["arboretum/hdr"];
	gfx::image_view cubemap_view = cubemap.view(gfx::imgv_type::image_cube);
	gfx::sampler sampler;

    gfx::compute_pipeline trace_pipeline(gfx::shader(gfx::shader_format::text, "11_pathtracer_incremental/trace.comp"));

    gfx::hbuffer<helper_info> helper_info_buffer(1);

    std::vector<gfx::descriptor_set> trace_descriptors(context->swapchain()->image_views().size());
    for (int i = 0; i < trace_descriptors.size(); ++i) {
        trace_descriptors[i].set(gfx::descriptor_type::uniform_buffer, 0, *camera_buffer);
        trace_descriptors[i].set(gfx::descriptor_type::uniform_buffer, 1, helper_info_buffer);
        trace_descriptors[i].set(gfx::descriptor_type::sampled_texture, 0, cubemap_view, sampler);
        trace_descriptors[i].set(gfx::descriptor_type::storage_image, 0, accumulation_cache_view);
        trace_descriptors[i].set(gfx::descriptor_type::storage_image, 1, bounce_cache_view);
        trace_descriptors[i].set(gfx::descriptor_type::storage_image, 2, direction_cache_view);
        trace_descriptors[i].set(gfx::descriptor_type::storage_image, 3, origin_cache_view);
        trace_descriptors[i].set(gfx::descriptor_type::storage_image, 4, counter_cache_view);
        trace_descriptors[i].set(gfx::descriptor_type::storage_image, 5, context->swapchain()->image_views()[i]);
    }
    gfx::commands cmd;
	gfx::fence update_fence;

    std::mt19937                          gen;
    std::uniform_real_distribution<float> dist(0.f, 1.f);

	int i=0;
    while (frame()) {
        ImGui::Begin("Settings");
		if (ImGui::Button("Reset Tracer"))
			i = 0;
        ImGui::End();

        helper_info_buffer[0].inverse_view_proj =
            glm::mat4(glm::mat3(glm::inverse(camera_buffer->at(0).view))) * inverse(camera_buffer->at(0).projection);
        helper_info_buffer[0].time = float(glfwGetTime()) * dist(gen);
		helper_info_buffer[0].reset = (i = (i+1) % 1);
		update_fence.wait();
        cmd.reset();
        cmd.bind_pipeline(trace_pipeline);
        cmd.bind_descriptors({&trace_descriptors[context->swapchain()->current_image()], 1});
        cmd.dispatch_compute((1280 + 15) / 16, (720 + 15) / 16);
        cmd.execute(update_fence);
    }
}