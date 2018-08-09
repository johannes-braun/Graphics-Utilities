#include <gfx/gfx.hpp>
#include "gfx/gui/imgui/imgui_internal.h"

int main()
{
    gfx::context_options opt;
    opt.graphics_api        = gfx::gapi::vulkan;
    opt.debug               = false;
    opt.use_window          = true;
    opt.framebuffer_samples = 8;

    auto context = gfx::context::create(opt);
    context->make_current();

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

    gfx::camera cam;
    cam.transform_mode.position = {0, 0, 3.f};
    gfx::camera_controller          controller;
    gfx::hbuffer<gfx::camera::data> cam_buffer(1);

    gfx::binding_set set1(layout1);
    set1.bind(0, cam_buffer);
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
    gfx::pipe_state::multisample msaa;
    msaa.sample_shading_enable = true;
    msaa.samples               = gfx::sample_count::x8;

    gfx::pipe_state g_state;
    g_state.state_bindings      = &layout;
    g_state.state_depth_stencil = &ds;
    g_state.state_multisample   = &msaa;
    g_state.state_vertex_input  = &input;

    gfx::renderpass_layout rpl(gfx::sample_count::x8);
    rpl.add_color_attachment(gfx::bgra8unorm);
    rpl.set_depth_stencil_attachment(gfx::d32f);

    gfx::graphics_pipeline pipeline(g_state, rpl,
                                    {gfx::shader{gfx::shader_type::vert, "05_implementation_tests/vert.vert"},
                                     gfx::shader{gfx::shader_type::frag, "05_implementation_tests/frag.frag"}});

    std::vector<gfx::framebuffer> fbos;
    std::vector<gfx::image>       deps;
    std::vector<gfx::image_view>  dep_vs;
    std::vector<gfx::image>       msaa_tex;
    std::vector<gfx::image_view>  msaa_tex_v;
    for (const auto& i : context->swapchain()->image_views()) {
        dep_vs.push_back(deps.emplace_back(gfx::img_type::attachment, gfx::d32f, gfx::extent{1280, 720}, gfx::sample_count::x8)
                             .view(gfx::imgv_type::image2d));
        msaa_tex_v.push_back(
            msaa_tex.emplace_back(gfx::img_type::attachment, gfx::bgra8unorm, gfx::extent{1280, 720}, gfx::sample_count::x8)
                .view(gfx::imgv_type::image2d));
        fbos.emplace_back(1280, 720, 1, rpl);
        fbos.back().attach(gfx::attachment::color, 0, msaa_tex_v.back(), glm::vec4(0.5f, 0.2f, 0.1f, 1.f));
        fbos.back().attach(gfx::attachment::resolve, 0, i, glm::vec4(0.5f, 0.2f, 0.1f, 1.f));
        fbos.back().attach(gfx::attachment::depth_stencil, 0, dep_vs.back(), gfx::depth_stencil(0.f, 0));
    }

    std::vector<gfx::commands> cmd3;
	for (int i = 0; i < fbos.size(); ++i)
	{
		cmd3.emplace_back(gfx::commands_type::graphics);
	}

	gfx::gui gui;
	gfx::image logo(gfx::himage(gfx::rgba8unorm, "ui/logo_next.png"));
	const auto logo_view = logo.view(gfx::imgv_type::image2d);

    float f      = 0;
    int   frames = 0;
    while (context->run()) {
        gui.new_frame();
        ImGui::Begin("Settings");

		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnWidth(0, 56);
        ImGui::Image(&logo_view, ImVec2(48, 48));
		ImGui::NextColumn();
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.8f, 1.f, 1.f));
		ImGui::PushFont(gui.font_huge());
		ImGui::Text("GFX");
		ImGui::PopFont();
		ImGui::PopStyleColor();
		ImGui::PushFont(gui.font_small());
		ImGui::Text("GRAPHICS UTILITIES");
		ImGui::PopFont();
		ImGui::Columns(1);
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Button("Click me!");
		ImGui::TextWrapped("This is a randomly brain-generated text to test fonts.");
        ImGui::End();

        f += context->delta();
        ++frames;
        if (f > 1.f) {
            glfwSetWindowTitle(context->window(), ("FPS: " + std::to_string(frames / f)).c_str());
            f      = 0;
            frames = 0;
        }

        controller.update(cam);
        cam_buffer[0] = cam.info();

        const gfx::u32 frame = context->swapchain()->current_image();
        cmd3[frame].reset();
        cmd3[frame].begin();
        cmd3[frame].begin_pass(fbos[frame]);
        cmd3[frame].bind_pipeline(pipeline, {&set1});
        cmd3[frame].bind_vertex_buffer(vertex_buffer, 0);
        cmd3[frame].bind_index_buffer(index_buffer, gfx::index_type::uint32);
        cmd3[frame].push_binding(1, 0, gfx::binding_type::sampled_image, texture2, sampler);
        for(const auto& g : scene.mesh.geometries)
            cmd3[frame].draw_indexed(g.index_count, 1, g.base_index, g.base_vertex);
        cmd3[frame].end_pass();
        cmd3[frame].end();
        cmd3[frame].execute();

        gui.render();
    }
}
