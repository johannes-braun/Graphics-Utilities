#include "context_opengl.hpp"
#include "swapchain_opengl.hpp"
#include <gfx/log.hpp>
#include <mygl/mygl.hpp>

namespace gfx::opengl
{
swapchain_implementation::swapchain_implementation()
        : _context(context::current())
{
    _window   = _context.lock()->window();
}

uint32_t swapchain_implementation::current_image() const noexcept { return _current_image; }

void swapchain_implementation::present()
{
    if(!_context.expired())
    {
        glfwSwapBuffers(_window);
        _current_image = (_current_image + 1) % 2;
    }
}

void swapchain_implementation::resize(uint32_t width, uint32_t height)
{
    glViewport(0, 0, width, height);
}

std::any swapchain_implementation::api_handle()
{
    return {};
}
}
