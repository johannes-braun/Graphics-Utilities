#define GFX_EXPOSE_APIS
#include <gfx/gfx.hpp>

int main()
{
    gfx::context_options opt;
    opt.window_title = "[13] Image Based Lighting";
    opt.debug        = true;
    auto context     = gfx::context::create(opt);
    context->make_current();

    gfx::image                  base_cubemap(gfx::img_type::image2d, gfx::rgba16f, gfx::extent(1024, 1024, 6), 10);
    const std::filesystem::path folder = "arboretum";
    base_cubemap.layer(0) << gfx::himage(base_cubemap.pixel_format(), folder / "hdr/posx.hdr");
    base_cubemap.layer(1) << gfx::himage(base_cubemap.pixel_format(), folder / "hdr/negx.hdr");
    base_cubemap.layer(2) << gfx::himage(base_cubemap.pixel_format(), folder / "hdr/posy.hdr");
    base_cubemap.layer(3) << gfx::himage(base_cubemap.pixel_format(), folder / "hdr/negy.hdr");
    base_cubemap.layer(4) << gfx::himage(base_cubemap.pixel_format(), folder / "hdr/posz.hdr");
    base_cubemap.layer(5) << gfx::himage(base_cubemap.pixel_format(), folder / "hdr/negz.hdr");
    base_cubemap.generate_mipmaps();
    gfx::image_view base_cubemap_view(gfx::imgv_type::image_cube, base_cubemap.pixel_format(), base_cubemap, 0, base_cubemap.levels(), 0,
                                      6);
    gfx::sampler    sampler;

    // --------------------------------------------------------------------------------------------------------------------
    // Generate Filtered Cubemap
    // --------------------------------------------------------------------------------------------------------------------

    constexpr uint32_t sampling_size = 32;
    gfx::image         filtered_cubemap(gfx::img_type::image2d, gfx::rgba16f, gfx::extent(sampling_size, sampling_size, 6), 1);
    gfx::image_view    filtered_cubemap_view(gfx::imgv_type::image_cube, filtered_cubemap.pixel_format(), filtered_cubemap, 0, 1, 0, 6);
    gfx::image         specular_cubemap(gfx::img_type::image2d, gfx::rgba16f, base_cubemap.extents(), base_cubemap.levels());
    gfx::image_view    specular_cubemap_view(gfx::imgv_type::image_cube, specular_cubemap.pixel_format(), specular_cubemap, 0,
                                          base_cubemap.levels(), 0, 6);

    mygl::framebuffer fbo;
    glCreateFramebuffers(1, &fbo);
    glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0, filtered_cubemap, 0);

    gfx::graphics_pipeline filter_pipeline{gfx::vertex_input(gfx::topology::point_list), gfx::state_info()};
    filter_pipeline.attach(gfx::shader_type::vert, gfx::shader(gfx::shader_format::text, "13_ibl/cubemap_filter.vert"));
    filter_pipeline.attach(gfx::shader_type::geom, gfx::shader(gfx::shader_format::text, "13_ibl/cubemap_filter.geom"));
    filter_pipeline.attach(gfx::shader_type::frag, gfx::shader(gfx::shader_format::text, "13_ibl/cubemap_filter_diffuse.frag"));

    gfx::graphics_pipeline specular_pipeline{gfx::vertex_input(gfx::topology::point_list), gfx::state_info()};
    specular_pipeline.attach(gfx::shader_type::vert, gfx::shader(gfx::shader_format::text, "13_ibl/cubemap_filter.vert"));
    specular_pipeline.attach(gfx::shader_type::geom, gfx::shader(gfx::shader_format::text, "13_ibl/cubemap_filter.geom"));
    specular_pipeline.attach(gfx::shader_type::frag,
                             gfx::shader(gfx::shader_format::text, "13_ibl/cubemap_filter_specular.frag"));

    gfx::descriptor_set filter_set;
    filter_set.set(gfx::descriptor_type::sampled_texture, 0, base_cubemap_view, sampler);
    gfx::commands cmd;

    gfx::clear_value clear_values[]{glm::vec4(1, 1, 1, 1), gfx::depth_stencil{0.f, 0}};
    gfx::viewport    filter_viewport(0, 0, sampling_size, sampling_size, 0.01f, 100.f);

    cmd.begin_pass(clear_values, 2, fbo);
    cmd.bind_descriptors(&filter_set, 1);
    cmd.bind_pipeline(filter_pipeline);
    cmd.set_viewports(&filter_viewport, 1, 0);
    cmd.draw(6, 1);
    cmd.execute();

    gfx::hbuffer<float> roughness(1);
    filter_set.set(gfx::descriptor_type::uniform_buffer, 0, roughness);

    for (int l = 0; l < 10; ++l)
    {
        glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0, specular_cubemap, l);
        roughness[0] = l / (9.f);
        gfx::viewport viewport(0, 0, 1024 >> l, 1024 >> l, 0.01f, 100.f);
        cmd.begin_pass(clear_values, 2, fbo);
        cmd.bind_descriptors(&filter_set, 1);
        cmd.bind_pipeline(specular_pipeline);
        cmd.set_viewports(&viewport, 1, 0);
        cmd.draw(6, 1);
        cmd.execute();

        glFinish();
    }

    // --------------------------------------------------------------------------------------------------------------------
    // Generate LUT for BRDF
    // --------------------------------------------------------------------------------------------------------------------

    gfx::image        brdf_lut(gfx::img_type::image2d, gfx::rg16f, gfx::extent(512, 512, 1), 1);
    gfx::image_view   brdf_lut_view(gfx::imgv_type::image_2d, brdf_lut.pixel_format(), brdf_lut, 0, 1, 0, 1);
    mygl::framebuffer lut_framebuffer;
    glCreateFramebuffers(1, &lut_framebuffer);
    glNamedFramebufferTexture(lut_framebuffer, GL_COLOR_ATTACHMENT0, brdf_lut, 0);

    gfx::viewport          lut_viewport(0, 0, 512, 512, 0.01f, 100.f);
    gfx::graphics_pipeline lut_pipeline{gfx::vertex_input(gfx::topology::triangle_list), gfx::state_info()};
    lut_pipeline.attach(gfx::shader_type::vert, gfx::shader(gfx::shader_format::text, "postfx/screen.vert"));
    lut_pipeline.attach(gfx::shader_type::frag, gfx::shader(gfx::shader_format::text, "13_ibl/brdf_lut.frag"));

    cmd.reset();
    cmd.begin_pass(clear_values, 1, lut_framebuffer);
    cmd.bind_pipeline(lut_pipeline);
    cmd.set_viewports(&lut_viewport, 1, 0);
    cmd.draw(3);
    cmd.execute();

    gfx::scene_file scene("bunny.dae");

    gfx::buffer<gfx::vertex3d> mesh_vertex_buffer(gfx::buffer_usage::vertex, scene.meshes[0].vertices);
    gfx::buffer<gfx::index32>  mesh_index_buffer(gfx::buffer_usage::index, scene.meshes[0].indices);

    const auto mesh_vertex_input = std::make_shared<gfx::vertex_input>();
    mesh_vertex_input->add_attribute(0, gfx::rgb32f, offsetof(gfx::vertex3d, position));
    mesh_vertex_input->add_attribute(0, gfx::rgb32f, offsetof(gfx::vertex3d, uv));
    mesh_vertex_input->add_attribute(0, gfx::rgb32f, offsetof(gfx::vertex3d, normal));
    mesh_vertex_input->set_binding_info(0, sizeof(gfx::vertex3d), gfx::input_rate::vertex);

    const auto mesh_render_state                         = std::make_shared<gfx::state_info>();
    mesh_render_state->multisample.sample_shading_enable = true;
    mesh_render_state->multisample.samples               = gfx::sample_count::x8;

    const auto cubemap_render_state                        = std::make_shared<gfx::state_info>(*mesh_render_state);
    cubemap_render_state->depth_stencil.depth_write_enable = false;

    gfx::graphics_pipeline mesh_pipeline(mesh_vertex_input, mesh_render_state);
    mesh_pipeline.attach(gfx::shader_type::vert, gfx::shader(gfx::shader_format::text, "13_ibl/mesh_unique.vert"));
    mesh_pipeline.attach(gfx::shader_type::frag, gfx::shader(gfx::shader_format::text, "13_ibl/mesh_unique.frag"));

    gfx::graphics_pipeline cubemap_pipeline(gfx::vertex_input{}, cubemap_render_state);
    cubemap_pipeline.attach(gfx::shader_type::vert, gfx::shader(gfx::shader_format::text, "13_ibl/skybox.vert"));
    cubemap_pipeline.attach(gfx::shader_type::frag, gfx::shader(gfx::shader_format::text, "13_ibl/skybox.frag"));

    gfx::camera                     camera;
    gfx::camera_controller          controller;
    gfx::hbuffer<gfx::camera::data> camera_buffer{camera.info()};

    gfx::descriptor_set mesh_cam_descriptor;
    mesh_cam_descriptor.set(gfx::descriptor_type::uniform_buffer, 0, camera_buffer);
    mesh_cam_descriptor.set(gfx::descriptor_type::sampled_texture, 0, filtered_cubemap_view, sampler);
    mesh_cam_descriptor.set(gfx::descriptor_type::sampled_texture, 1, specular_cubemap_view, sampler);
    mesh_cam_descriptor.set(gfx::descriptor_type::sampled_texture, 2, brdf_lut_view, sampler);

    gfx::descriptor_set cubemap_set;
    cubemap_set.set(gfx::descriptor_type::uniform_buffer, 0, camera_buffer);
    cubemap_set.set(gfx::descriptor_type::sampled_texture, 0, base_cubemap_view, sampler);

    // TODO: Framebuffers
    mygl::framebuffer msaa_fbo, resolve_fbo;
    glCreateFramebuffers(1, &msaa_fbo);
    glCreateFramebuffers(1, &resolve_fbo);

    gfx::image msaa_color_image(gfx::img_type::image2d, gfx::rgba16f, gfx::extent(1280, 720, 1), gfx::sample_count::x8);
    gfx::image msaa_depth_image(gfx::img_type::image2d, gfx::d32f, gfx::extent(1280, 720, 1), gfx::sample_count::x8);
    gfx::image resolve_image(gfx::img_type::image2d, gfx::rgba16f, gfx::extent(1280, 720, 1), 1);

    glNamedFramebufferTexture(msaa_fbo, GL_COLOR_ATTACHMENT0, msaa_color_image, 0);
    glNamedFramebufferTexture(msaa_fbo, GL_DEPTH_ATTACHMENT, msaa_depth_image, 0);
    glNamedFramebufferTexture(resolve_fbo, GL_COLOR_ATTACHMENT0, resolve_image, 0);

    glNamedFramebufferDrawBuffer(msaa_fbo, GL_COLOR_ATTACHMENT0);
    glNamedFramebufferReadBuffer(msaa_fbo, GL_COLOR_ATTACHMENT0);
    glNamedFramebufferDrawBuffer(resolve_fbo, GL_COLOR_ATTACHMENT0);
    glNamedFramebufferReadBuffer(resolve_fbo, GL_COLOR_ATTACHMENT0);

    gfx::viewport main_viewport(0, 0, 1280, 720, 0.01f, 100.f);
    cmd.reset();
    cmd.begin_pass(clear_values, 2, msaa_fbo);    // whatever
    cmd.bind_descriptors(&cubemap_set, 1);
    cmd.set_viewports(&main_viewport, 1, 0);
    cmd.bind_pipeline(cubemap_pipeline);
    cmd.draw(3);

    cmd.bind_descriptors(&mesh_cam_descriptor, 1);
    cmd.bind_pipeline(mesh_pipeline);
    cmd.set_viewports(&main_viewport, 1, 0);
    cmd.bind_vertex_buffer(0, mesh_vertex_buffer);
    cmd.bind_index_buffer(mesh_index_buffer, gfx::index_type::uint32);
    cmd.draw_indexed(mesh_index_buffer.capacity());

    while (context->run())
    {
        controller.update(camera);
        camera_buffer[0] = camera.info();

        cmd.execute();

        glBlitNamedFramebuffer(msaa_fbo, resolve_fbo, 0, 0, 1280, 720, 0, 0, 1280, 720, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        glBlitNamedFramebuffer(resolve_fbo, mygl::framebuffer::zero, 0, 0, 1280, 720, 0, 0, 1280, 720, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }
}