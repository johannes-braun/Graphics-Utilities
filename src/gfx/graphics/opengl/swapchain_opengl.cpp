#include "context_opengl.hpp"
#include "swapchain_opengl.hpp"
#include <gfx/log.hpp>
#include <mygl/mygl.hpp>

namespace gfx::opengl
{
swapchain_implementation::swapchain_implementation()
        : _context(context::current())
{
	auto sp = _context.lock();
    _window   = sp->window();
	_image_count = sp->options().framebuffer_images;
	swapchain_implementation::resize(sp->options().window_width, sp->options().window_height);
}

uint32_t swapchain_implementation::current_image() const noexcept { return _current_image; }

void swapchain_implementation::present()
{
    if(!_context.expired())
    {
		const auto ext = _images[_current_image].extents();
		glBindFramebuffer(GL_FRAMEBUFFER, mygl::framebuffer::zero);
		glDrawBuffer(GL_FRONT);
		glBlitNamedFramebuffer(handle_cast<mygl::framebuffer>(_blit_helpers[_current_image]), mygl::framebuffer::zero, 
			0, 0, ext.width, ext.height,
			0, 0, ext.width, ext.height,
			GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glFlush();
       // glfwSwapBuffers(_window);
        _current_image = (_current_image + 1) % _image_count;
    }
}

void swapchain_implementation::resize(uint32_t width, uint32_t height)
{
    glViewport(0, 0, width, height);

	_images.clear();
	_image_views.clear();
	_blit_helpers.clear();
    for(int i=0; i < _image_count; ++i)
    {
		_images.emplace_back(img_type::image2d, rgba8unorm, extent(width, height, 1), 1);
		_image_views.emplace_back(imgv_type::image_2d, _images[i]);
		_blit_helpers.emplace_back(width, height);
		_blit_helpers.back().attach(gfx::attachment::color, 0, _image_views.back());
    }
	_current_image = 0;
}

std::any swapchain_implementation::api_handle()
{
    return {};
}

const std::vector<image_view>& swapchain_implementation::image_views() const
{
	return _image_views;
}
}
