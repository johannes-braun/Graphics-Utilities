#define GFX_EXPOSE_APIS
#include <gfx/gfx.hpp>
#include <iostream>

#include <vulkan/vulkan.hpp>

#include "gfx/graphics/opengl/formats.hpp"
#include <gfx/graphics/graphics_pipeline.hpp>
#include <gfx/graphics/opengl/binding_set_opengl.hpp>
#include <gfx/graphics/vulkan/framebuffer_vulkan.hpp>

int main()
{
	const auto then = std::chrono::steady_clock::now();
    gfx::context_options opt;
    opt.graphics_api = gfx::gapi::vulkan;
    opt.debug        = true;
    opt.use_window   = true;

    auto context = gfx::context::create(opt);
    context->make_current();

    //// Buffer
    gfx::hbuffer<float> x{1.f, 23.f, 29.f};
    gfx::hbuffer<float> src = std::move(x);
    gfx::buffer<float>  dst(gfx::buffer_usage::uniform, {2.f, 1.f, 0.5f, 10.f, 1.f, 9.f});
    dst << src;

    gfx::buf_copy(src, dst, dst.capacity());

    for (auto f : src) gfx::ilog << f;

    // Images
    const auto cubemap_format = gfx::rgba8unorm;
    const auto info           = gfx::image_file::info("hdri/hdr/posx.hdr");
    gfx::image file_texture(gfx::img_type::image2d, cubemap_format, gfx::extent(info.width, info.height, 6), 11);
    file_texture.layer(0) << gfx::host_image(cubemap_format, gfx::image_file("hdri/hdr/posx.hdr", gfx::bits::b32, 3));
    file_texture.layer(1) << gfx::host_image(cubemap_format, gfx::image_file("hdri/hdr/negx.hdr", gfx::bits::b32, 3));
    file_texture.layer(2) << gfx::host_image(cubemap_format, gfx::image_file("hdri/hdr/posy.hdr", gfx::bits::b32, 3));
    file_texture.layer(3) << gfx::host_image(cubemap_format, gfx::image_file("hdri/hdr/negy.hdr", gfx::bits::b32, 3));
    file_texture.layer(4) << gfx::host_image(cubemap_format, gfx::image_file("hdri/hdr/posz.hdr", gfx::bits::b32, 3));
    file_texture.layer(5) << gfx::host_image(cubemap_format, gfx::image_file("hdri/hdr/negz.hdr", gfx::bits::b32, 3));
    file_texture.generate_mipmaps();

    gfx::image another_image(gfx::himage(gfx::rgba8unorm, gfx::image_file("Lena.png", gfx::bits::b8, 4)));

    auto cubemap = file_texture.view(gfx::imgv_type::image_cube);
    auto texture = another_image.view(gfx::imgv_type::image2d);

    // Samplers
    gfx::sampler sampler;
    sampler.set_anisotropy(true, 16.f);
    sampler.set_filter(gfx::filter_mode::min, gfx::filter::linear);
    sampler.set_filter(gfx::filter_mode::mag, gfx::filter::linear);
    sampler.set_filter(gfx::filter_mode::mipmap, gfx::filter::linear);
    sampler.set_wrap(gfx::wrap::u, gfx::wrap_mode::mirror_repeat);
    sampler.set_wrap(gfx::wrap::v, gfx::wrap_mode::mirror_repeat);
    sampler.set_wrap(gfx::wrap::w, gfx::wrap_mode::mirror_repeat);

    // TODO: for building/creating a sampler it must be casted once.
	sampler.api_handle();

    gfx::binding_layout layout1;
    layout1
        .push(gfx::binding_type::uniform_buffer)    // VK: { set: 0, binding: 0 }; GL: { uniform buffer binding: 0 }
        .push(gfx::binding_type::sampled_image, 2)     // VK: { set: 0, binding: 1 }; GL: { texture binding: 0 }
        .push(gfx::binding_type::sampled_image)     // VK: { set: 0, binding: 2 }; GL: { texture binding: 1 }
        .push(gfx::binding_type::storage_image);    // VK: { set: 0, binding: 3 }; GL: { image binding: 0 }
    gfx::binding_layout layout2;
    layout2
        .push(gfx::binding_type::uniform_buffer)    // VK: { set: 1, binding: 0, arr:0, 1 }; GL: { uniform buffer binding: 1, 2 }
        .push(gfx::binding_type::uniform_buffer)       // VK: { set: 1, binding: 1 }; GL: { uniform buffer binding: 3 }
        .push(gfx::binding_type::storage_buffer);      // VK: { set: 1, binding: 0 }; GL: { uniform buffer binding: 0

    gfx::binding_set set1(layout1);
    set1.bind(0, dst);
    set1.bind(1, 0, cubemap, sampler);
	set1.bind(1, 1, cubemap, sampler);
    set1.bind(2, texture, sampler);
    set1.bind(3, texture);

    gfx::binding_set set2(layout2);
    set2.bind(0, dst);
    set2.bind(1, src);
    set2.bind(2, src);

    gfx::v2::renderpass_layout rpl;
    rpl.add_color_attachment(gfx::rgba8unorm);
    rpl.set_depth_stencil_attachment(gfx::d32f);

	gfx::v2::pipeline_state::layout layout;
	layout.binding_layouts.push_back(&layout1);
    gfx::v2::pipeline_state g_state;
	g_state.state_bindings = &layout;

	gfx::v2::graphics_pipeline pipeline(g_state, rpl, {
		gfx::shader{ gfx::shader_type::vert, "05_implementation_tests/vert.vert" },
		gfx::shader{ gfx::shader_type::frag, "05_implementation_tests/frag.frag" }
	});

	gfx::v2::compute_pipeline cp({}, gfx::shader{ gfx::shader_type::comp, "05_implementation_tests/comp.comp" });

	gfx::image col(gfx::img_type::attachment, gfx::rgba8unorm, { 1280, 720 }, 1);
	gfx::image dep(gfx::img_type::attachment, gfx::d32f, { 1280, 720 }, 1);
	gfx::image_view att_col = col.view(gfx::imgv_type::image2d);
	gfx::image_view att_dep = dep.view(gfx::imgv_type::image2d);
	gfx::framebuffer fbo(1280, 720, 1, rpl);
	fbo.attach(gfx::attachment::color, 0, att_col, glm::vec4(0.5f, 0.2f, 0.1f, 1.f));
	fbo.attach(gfx::attachment::depth_stencil, 0, att_dep, gfx::depth_stencil(0.f, 0));

	auto beg = static_cast<gfx::vulkan::framebuffer_implementation*>(&*fbo.implementation())->begin_info();

	gfx::file::message("Initialization", "Finished in " + std::to_string((std::chrono::steady_clock::now() - then).count()) + "ns", gfx::msg_type::ok, gfx::msg_icon::info);

    // CMDbuf
    /*
    - primary cmd buffer
        -> enqueue secondary cmd buffers. Different types:
            -> render commands
                -> "enqueued" into primary cmd buf (aka push_back into vector on execute()ion time)
                -> runs graphics and compute commands
    Or: all cmd bufs have semaphores. Every time a cmd is executed, the submit will sync on the last semaphore and append the current as signal sem.
    Then: non-synched, collect semaphores and wait for all? cross-synching like below?
            -> non-render commands (e.g. async compute pipeline)
                -> enqueued directly into compute queue.
                -> only runs compute commands (also synched on semaphores, but different ones. maybe cross-sync ala cmd.begin_after(cmd1, fbo) vs unsynched cmd.begin(fbo)?)

    abstract:
        imgui.render(cmd);
        ...
        mesh.render(cmd);
        etc...
     */
    enum class commands_type
    {
        graphics,
        compute
    };


    while (context->run()) 
	{

    }

    std::cin.ignore();
}
