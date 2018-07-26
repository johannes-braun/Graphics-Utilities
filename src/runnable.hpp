#pragma once

#define GFX_EXPOSE_APIS
#include "gfx/gfx.hpp"

class runnable
{
public:
	runnable()
	{
		glfwWindowHint(GLFW_DOUBLEBUFFER, false);
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
        //  tmp
		glBindFramebuffer(GL_FRAMEBUFFER, gfx::handle_cast<mygl::framebuffer>(main_framebuffer()));
		imgui->render();
	}

private:
	void build_framebuffer(uint32_t width, uint32_t height)
	{
		main_viewport = gfx::viewport(0, 0, width, height, 0.f, 1.f);
		glViewport(0, 0, width, height);
		_fbs.clear();
		_fb_attachments.clear();
		_fb_attachment_views.clear();
		if (_opt.framebuffer_samples == 1) {
			for (int i=0; i<_opt.framebuffer_images; ++i)
			{
				_fbs.emplace_back(std::make_unique<gfx::framebuffer>(width, height));
				_fbs.back()->attach(gfx::attachment::color, 0, context->swapchain()->image_views()[i], glm::vec4(0.2f, 0.2f, 0.2f, 1.f));

				_fb_attachments.emplace_back(std::make_unique<gfx::image>(gfx::img_type::image2d, gfx::d32f, gfx::extent(width, height, 1), gfx::sample_count(_opt.framebuffer_samples)));
				_fb_attachment_views.emplace_back(std::make_unique<gfx::image_view>(gfx::imgv_type::image2d, *_fb_attachments.back()));
				_fbs.back()->attach(gfx::attachment::depth_stencil, 0, *_fb_attachment_views.back(), gfx::depth_stencil{ 0.f, 0 });
			}
		}
		else
		{
            for(int i=0; i<_opt.framebuffer_images; ++i)
            {
				_fbs.emplace_back(std::make_unique<gfx::framebuffer>(width, height));

				_fb_attachments.emplace_back(std::make_unique<gfx::image>(gfx::img_type::image2d, gfx::rgba16f, gfx::extent(width, height, 1), gfx::sample_count(_opt.framebuffer_samples)));
                _fb_attachment_views.emplace_back(std::make_unique<gfx::image_view>(gfx::imgv_type::image2d, *_fb_attachments.back()));
				_fbs.back()->attach(gfx::attachment::color, 0, *_fb_attachment_views.back(), glm::vec4(0.2f, 0.2f, 0.2f, 1.f));
				
                _fbs.back()->attach(gfx::attachment::resolve, 0, context->swapchain()->image_views()[i]);

				_fb_attachments.emplace_back(std::make_unique<gfx::image>(gfx::img_type::image2d, gfx::d32f, gfx::extent(width, height, 1), gfx::sample_count(_opt.framebuffer_samples)));
				_fb_attachment_views.emplace_back(std::make_unique<gfx::image_view>(gfx::imgv_type::image2d, *_fb_attachments.back()));
				_fbs.back()->attach(gfx::attachment::depth_stencil, 0, *_fb_attachment_views.back(), gfx::depth_stencil{0.f, 0});
			}
		}
	}

	std::shared_ptr<gfx::context>                    context;
	std::unique_ptr<gfx::imgui>                      imgui;
	gfx::framebuffer&                main_framebuffer() { return *_fbs[context->swapchain()->current_image()]; }
	std::unique_ptr<gfx::hbuffer<gfx::camera::data>> camera_buffer;
	gfx::camera                                      camera;
	gfx::camera_controller                           controller;
	gfx::viewport                                    main_viewport;

	std::vector<std::unique_ptr<gfx::image>>      _fb_attachments;
	std::vector<std::unique_ptr<gfx::image_view>> _fb_attachment_views;
	std::vector<std::unique_ptr<gfx::framebuffer>> _fbs;
	gfx::context_options                          _opt;
};

#ifndef GFX_RUNNABLE_DISABLE_MAIN
int main()
{
	runnable().run();
}
#endif