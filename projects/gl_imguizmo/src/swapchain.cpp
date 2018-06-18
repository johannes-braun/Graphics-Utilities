#include "swapchain.hpp"
#include "context.hpp"

#include "swapchain_opengl.hpp"
#include "swapchain_vulkan.hpp"

namespace gfx
{
std::unique_ptr<detail::swapchain_implementation> detail::make_swapchain_implementation()
{
    switch(context::current()->graphics_api)
    {
    case gapi::opengl:
        return std::make_unique<opengl::swapchain_implementation>();
    case gapi::vulkan:
        return std::make_unique<vulkan::swapchain_implementation>();
    default:
        break;
    }
    return nullptr;
}

swapchain::swapchain() { _implementation = detail::make_swapchain_implementation(); }

uint32_t swapchain::current_image() const noexcept { return _implementation->current_image(); }

void swapchain::present() { _implementation->present(); }
} // namespace gfx
