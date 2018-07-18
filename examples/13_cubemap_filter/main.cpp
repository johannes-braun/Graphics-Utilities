#define GFX_EXPOSE_APIS
#include <gfx/gfx.hpp>

int main()
{
    gfx::context_options opt;
    opt.window_title = "[13] Cubemap Filter";
    opt.debug        = true;
    auto context     = gfx::context::create(opt);
    context->make_current();

    gfx::image base_cubemap(gfx::img_type::image2d, gfx::rgba16f, gfx::extent(1024, 1024, 6), 3);
    base_cubemap.layer(0) << gfx::himage(base_cubemap.pixel_format(), "hdri/hdr/posx.hdr");
    base_cubemap.layer(1) << gfx::himage(base_cubemap.pixel_format(), "hdri/hdr/negx.hdr");
    base_cubemap.layer(2) << gfx::himage(base_cubemap.pixel_format(), "hdri/hdr/posy.hdr");
    base_cubemap.layer(3) << gfx::himage(base_cubemap.pixel_format(), "hdri/hdr/negy.hdr");
    base_cubemap.layer(4) << gfx::himage(base_cubemap.pixel_format(), "hdri/hdr/posz.hdr");
    base_cubemap.layer(5) << gfx::himage(base_cubemap.pixel_format(), "hdri/hdr/negz.hdr");
    base_cubemap.generate_mipmaps();
    gfx::image_view base_cubemap_view(gfx::imgv_type::image_cube, base_cubemap.pixel_format(), base_cubemap, 2, 1, 0, 6);
    gfx::sampler    sampler;

	constexpr uint32_t sampling_size = 1024;
    gfx::image         filtered_cubemap(gfx::img_type::image2d, gfx::rgba16f, gfx::extent(sampling_size, sampling_size, 6), 1);

    mygl::framebuffer fbo;
    glCreateFramebuffers(1, &fbo);
    glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0, filtered_cubemap, 0);

    gfx::graphics_pipeline filter_pipeline{gfx::vertex_input(gfx::topology::point_list), gfx::state_info()};
    filter_pipeline.attach(gfx::shader_type::vert, gfx::shader(gfx::shader_format::text, "13_cubemap_filter/diffuse.vert"));
    filter_pipeline.attach(gfx::shader_type::geom, gfx::shader(gfx::shader_format::text, "13_cubemap_filter/diffuse.geom"));
    filter_pipeline.attach(gfx::shader_type::frag, gfx::shader(gfx::shader_format::text, "13_cubemap_filter/diffuse.frag"));

    gfx::descriptor_set    filter_set;
    filter_set.set(gfx::descriptor_type::sampled_texture, 0, base_cubemap_view, sampler);
    gfx::commands cmd;

    gfx::clear_value clear_values[]{glm::vec4(1, 1, 1, 1), gfx::depth_stencil{0.f, 0}};
    gfx::viewport    viewport(0, 0, sampling_size, sampling_size, 0.01f, 100.f);

	cmd.begin_pass(clear_values, 2, fbo);
    cmd.bind_descriptors(&filter_set, 1);
    cmd.bind_pipeline(filter_pipeline);
    cmd.set_viewports(&viewport, 1, 0);
    cmd.draw(6, 1);
    cmd.execute();

    while (context->run()) {
    }
}