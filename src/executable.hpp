#pragma once
#include <gfx/gfx.hpp>

struct executable
{
	static void init(gfx::context_options& opt);
	void run();

	executable();
	bool frame();

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
	gfx::framebuffer*                current_framebuffer = nullptr;

	gfx::ecs::ecs ecs;
	gfx::user_camera_system input_system;
	gfx::ecs::unique_entity user_entity;

	gfx::ecs::system_list control_systems;

	std::unique_ptr<gfx::hbuffer<gfx::camera_component::matrices>> camera_buffer;
};

#ifndef GFX_EXECUTABLE_NO_MAIN
int main()
{
	executable().run();
	return 0;
}
#endif