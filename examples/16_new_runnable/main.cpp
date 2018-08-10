#include <gfx/gfx.hpp>
#include <executable.hpp>

void executable::init(gfx::context_options& opt)
{
    opt.graphics_api        = gfx::gapi::vulkan;
    opt.debug               = true;
    opt.use_window          = true;
    opt.framebuffer_samples = gfx::sample_count::x8;
    opt.window_title        = "[16] Executable";
}

void executable::run()
{
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

    gfx::image another_image2(gfx::himage(gfx::rgba8unorm, gfx::image_file("grid.jpg", gfx::bits::b8, 4)));

    const auto         cubemap  = file_texture.view(gfx::imgv_type::image_cube);
    const auto         texture2 = another_image2.view(gfx::imgv_type::image2d);
    const gfx::sampler sampler;

    gfx::binding_layout layout1;
    layout1.push(gfx::binding_type::uniform_buffer).push(gfx::binding_type::sampled_image);

    gfx::binding_layout push_layout(true);
    push_layout.push(gfx::binding_type::sampled_image);

    gfx::binding_set set1(layout1);
    set1.bind(0, *camera_buffer);
    set1.bind(1, cubemap, sampler);

    gfx::scene_file            scene("bunny.dae");
    gfx::buffer<gfx::vertex3d> vertex_buffer(gfx::buffer_usage::vertex, scene.mesh.vertices);
    gfx::buffer<gfx::index32>  index_buffer(gfx::buffer_usage::index, scene.mesh.indices);

    gfx::pipe_state::vertex_input input;
    input.attributes.emplace_back(0, 0, gfx::rgb32f, offsetof(gfx::vertex3d, position));
    input.attributes.emplace_back(1, 0, gfx::rg32f, offsetof(gfx::vertex3d, uv));
    input.attributes.emplace_back(2, 0, gfx::rgb32f, offsetof(gfx::vertex3d, normal));
    input.bindings.emplace_back(0, sizeof(gfx::vertex3d));

    gfx::pipe_state::depth_stencil ds;
    ds.depth_test_enable = true;
    gfx::pipe_state::binding_layouts layout;
    layout.layouts.push_back(&layout1);
    layout.layouts.push_back(&push_layout);
	gfx::pipe_state::rasterizer raster;
	raster.cull = gfx::cull_mode::back;

    gfx::pipe_state g_state;
    g_state.state_bindings      = &layout;
    g_state.state_depth_stencil = &ds;
    g_state.state_multisample   = &msaa_state;
    g_state.state_vertex_input  = &input;
	g_state.state_rasterizer = &raster;

    gfx::graphics_pipeline pipeline(g_state, pass_layout,
                                    {gfx::shader{gfx::shader_type::vert, "05_implementation_tests/vert.vert"},
                                     gfx::shader{gfx::shader_type::frag, "05_implementation_tests/frag.frag"}});

    while (frame()) {
        //ImGui::Begin("Settings");
        //ImGui::Button("Click me!");
        //ImGui::TextWrapped("This is a randomly brain-generated text to test fonts.");
        //ImGui::End();

        current_command->begin_pass(*current_framebuffer);
        current_command->bind_pipeline(pipeline, {&set1});
        current_command->bind_vertex_buffer(vertex_buffer, 0);
        current_command->bind_index_buffer(index_buffer, gfx::index_type::uint32);
        current_command->push_binding(1, 0, gfx::binding_type::sampled_image, texture2, sampler);
        for (const auto& g : scene.mesh.geometries) current_command->draw_indexed(g.index_count, 1, g.base_index, g.base_vertex);
        current_command->end_pass();
    }
}
