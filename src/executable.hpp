#pragma once
#include <gfx/gfx.hpp>

struct executable
{
	executable()
	{
		init(options);
		context = gfx::context::create(options);
		context->make_current();

		pass_layout = gfx::renderpass_layout(options.framebuffer_samples);
		pass_layout.add_color_attachment(gfx::bgra8unorm);
		pass_layout.set_depth_stencil_attachment(gfx::d32f);

		msaa_state.sample_shading_enable = true;
		msaa_state.samples               = options.framebuffer_samples;

		auto* cb = context->framebuffer_size_callback.add([this](GLFWwindow* w, int x, int y) {
			fbos.clear();
			depth_attachments.clear();
			depth_attachment_views.clear();
			msaa_attachments.clear();
			msaa_attachment_views.clear();
			for (const auto& i : context->swapchain()->image_views()) {
				depth_attachment_views.push_back(
					depth_attachments.emplace_back(gfx::img_type::attachment, gfx::d32f, gfx::extent(x, y), options.framebuffer_samples)
					.view(gfx::imgv_type::image2d));
				fbos.emplace_back(x, y, 1, pass_layout);

				if (options.framebuffer_samples == gfx::sample_count::x1) {
					fbos.back().attach(gfx::attachment::color, 0, i, glm::vec4(0.2f, 0.2f, 0.2f, 1.f));
				}
				else
				{
					msaa_attachment_views.push_back(
						msaa_attachments
						.emplace_back(gfx::img_type::attachment, gfx::bgra8unorm, gfx::extent(x, y), options.framebuffer_samples)
						.view(gfx::imgv_type::image2d));
					fbos.back().attach(gfx::attachment::color, 0, msaa_attachment_views.back(), glm::vec4(0.2f, 0.2f, 0.2f, 1.f));
					fbos.back().attach(gfx::attachment::resolve, 0, i);
				}
				fbos.back().attach(gfx::attachment::depth_stencil, 0, depth_attachment_views.back(), gfx::depth_stencil(0.f, 0));
			}
		});
		cb->operator()(context->window(), options.window_width, options.window_height);

		for (int i = 0; i < fbos.size(); ++i) {
			render_commands.emplace_back(gfx::commands_type::graphics);
		}

		gui       = std::make_unique<gfx::gui>();
		logo      = std::make_unique<gfx::image>(gfx::himage(gfx::rgba8unorm, "ui/logo_next.png"));
		logo_view = std::make_unique<gfx::image_view>(gfx::imgv_type::image2d, *logo);
		camera_buffer = std::make_unique<gfx::buffer<gfx::camera::data>>(gfx::buffer_usage::uniform, 1);
	}

	bool frame()
	{
		if (current_command)
		{
			current_command->render(*gui);
			current_command->end();
			current_command->execute();
		}

		bool rv = context->run();

		if (rv) {
			gui->new_frame();
			static bool open = false;
			ImGui::Begin("Settings", &open);
			ImGui::Columns(2, nullptr, false);
			ImGui::SetColumnWidth(0, 56);
			ImGui::Image(&*logo_view, ImVec2(48, 48));
			ImGui::NextColumn();
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.8f, 1.f, 1.f));
			ImGui::PushFont(gui->font_huge());
			ImGui::Text("GFX");
			ImGui::PopFont();
			ImGui::PopStyleColor();
			ImGui::PushFont(gui->font_small());
			ImGui::Text("GRAPHICS UTILITIES");
			ImGui::PopFont();
			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::Spacing();
			ImGui::End();

			controller.update(camera);

			const gfx::u32 frame = context->swapchain()->current_image();
			current_command      = &render_commands[frame];
			current_framebuffer  = &fbos[frame];
			current_command->reset();
			current_command->begin();
			current_command->update_buffer(*camera_buffer, 0, camera.info());
		}

		return rv;
	}

	static void init(gfx::context_options& opt);
	void run();

	gfx::ressources res;
	gfx::context_options             options;
	std::shared_ptr<gfx::context>    context;
	gfx::renderpass_layout           pass_layout;
	gfx::pipe_state::multisample     msaa_state;
	std::vector<gfx::framebuffer>    fbos;
	std::vector<gfx::image>          depth_attachments;
	std::vector<gfx::image_view>     depth_attachment_views;
	std::vector<gfx::image>          msaa_attachments;
	std::vector<gfx::image_view>     msaa_attachment_views;
	std::vector<gfx::commands>       render_commands;
	std::unique_ptr<gfx::gui>        gui;
	std::unique_ptr<gfx::image>      logo;
	std::unique_ptr<gfx::image_view> logo_view;
	gfx::commands*                   current_command = nullptr;
	gfx::framebuffer*                current_framebuffer;

	gfx::camera camera;
	gfx::camera_controller          controller;
	std::unique_ptr<gfx::buffer<gfx::camera::data>> camera_buffer;
};

#ifndef GFX_EXECUTABLE_NO_MAIN
int main()
{
	executable().run();
	return 0;
}
#endif