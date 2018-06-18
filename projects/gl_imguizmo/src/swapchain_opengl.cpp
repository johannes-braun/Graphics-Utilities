#include "swapchain_opengl.hpp"
#include <gfx/log.hpp>
#include "context_opengl.hpp"

namespace gfx::opengl
{
swapchain_implementation::swapchain_implementation()
{
    auto& ctx  = context::current();
    _window         = ctx->window();
}

uint32_t swapchain_implementation::current_image() const noexcept
{ return _current_image; }

void swapchain_implementation::present()
{
    glfwSwapBuffers(_window);
    ++_current_image;
}
}
