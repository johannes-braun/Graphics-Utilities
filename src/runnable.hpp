#pragma once

#define GFX_EXPOSE_APIS
#include "gfx/gfx.hpp"

class runnable
{
public:
	runnable()
	{
		init(_opt);
		context = gfx::context::create(_opt);
		context->make_current();

		build_framebuffer(_opt.window_width, _opt.window_height);

		context->framebuffer_size_callback.add([this](GLFWwindow*, int w, int h) { build_framebuffer(w, h); });

		imgui         = std::make_unique<gfx::imgui>();
		camera_buffer = std::make_unique<gfx::hbuffer<gfx::camera::data>>(1);
	}
	void init(gfx::context_options& options);
	void run();

protected:
    bool frame()
    {
		static bool b = false;
		if (b)
			end_frame();
		b = true;
		return begin_frame();
    }

	bool begin_frame()
	{
		const bool rv = context->run();
		if (rv) imgui->new_frame();

		controller.update(camera);
		camera_buffer->at(0) = camera.info();
		return rv;
	}

	void end_frame()
	{
		glBlitNamedFramebuffer(main_framebuffer->resolver(), mygl::framebuffer::zero, main_viewport.x, main_viewport.y, main_viewport.width,
			main_viewport.height, main_viewport.x, main_viewport.y, main_viewport.width, main_viewport.height,
			GL_COLOR_BUFFER_BIT, GL_NEAREST);
		imgui->render();
	}

private:
	void build_framebuffer(uint32_t width, uint32_t height)
	{
		main_viewport = gfx::viewport(0, 0, width, height, 0.f, 1.f);
		glViewport(0, 0, width, height);
		_fb_attachments.resize(3);
		_fb_attachment_views.resize(3);
		if (_opt.framebuffer_samples == 1) {
			_fb_attachments[0] = nullptr;
			_fb_attachments[1] = std::make_unique<gfx::image>(gfx::img_type::image2d, gfx::d32f, gfx::extent(width, height, 1), 1);
			_fb_attachments[2] = std::make_unique<gfx::image>(gfx::img_type::image2d, gfx::rgba16f, gfx::extent(width, height, 1), 1);

			_fb_attachment_views[0] = nullptr;
			_fb_attachment_views[1] = std::make_unique<gfx::image_view>(gfx::imgv_type::image_2d, *_fb_attachments[1]);
			_fb_attachment_views[2] = std::make_unique<gfx::image_view>(gfx::imgv_type::image_2d, *_fb_attachments[2]);

			main_framebuffer = std::make_unique<gfx::framebuffer>(width, height);
			main_framebuffer->attach(gfx::attachment::color, 0, *_fb_attachment_views[2], glm::vec4(0.2f, 0.2f, 0.2f, 1.f));
			main_framebuffer->attach(gfx::attachment::depth_stencil, 0, *_fb_attachment_views[1], gfx::depth_stencil{0.f, 0});
			main_framebuffer->detach(gfx::attachment::resolve, 0);
		}
		else
		{
			_fb_attachments[0] =
				std::make_unique<gfx::image>(gfx::img_type::image2d, gfx::rgba16f, gfx::extent(width, height, 1), gfx::sample_count(_opt.framebuffer_samples));
			_fb_attachments[1] =
				std::make_unique<gfx::image>(gfx::img_type::image2d, gfx::d32f, gfx::extent(width, height, 1), gfx::sample_count(_opt.framebuffer_samples));
			_fb_attachments[2] = std::make_unique<gfx::image>(gfx::img_type::image2d, gfx::rgba16f, gfx::extent(width, height, 1), 1);

			_fb_attachment_views[0] = std::make_unique<gfx::image_view>(gfx::imgv_type::image_2d, *_fb_attachments[0]);
			_fb_attachment_views[1] = std::make_unique<gfx::image_view>(gfx::imgv_type::image_2d, *_fb_attachments[1]);
			_fb_attachment_views[2] = std::make_unique<gfx::image_view>(gfx::imgv_type::image_2d, *_fb_attachments[2]);

			main_framebuffer = std::make_unique<gfx::framebuffer>(width, height);
			main_framebuffer->attach(gfx::attachment::color, 0, *_fb_attachment_views[0], glm::vec4(0.2f, 0.2f, 0.2f, 1.f));
			main_framebuffer->attach(gfx::attachment::depth_stencil, 0, *_fb_attachment_views[1], gfx::depth_stencil{0.f, 0});
			main_framebuffer->attach(gfx::attachment::resolve, 0, *_fb_attachment_views[2]);
		}
	}

	std::shared_ptr<gfx::context>                    context;
	std::unique_ptr<gfx::framebuffer>                main_framebuffer;
	std::unique_ptr<gfx::imgui>                      imgui;
	std::unique_ptr<gfx::hbuffer<gfx::camera::data>> camera_buffer;
	gfx::camera                                      camera;
	gfx::camera_controller                           controller;
	gfx::viewport                                    main_viewport;

	std::vector<std::unique_ptr<gfx::image>>      _fb_attachments;
	std::vector<std::unique_ptr<gfx::image_view>> _fb_attachment_views;
	gfx::context_options                          _opt;
};