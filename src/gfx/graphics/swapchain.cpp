#include <gfx/context.hpp>
#include <gfx/graphics/swapchain.hpp>

#include "opengl/swapchain_opengl.hpp"
//#include "swapchain_vulkan.hpp"

namespace gfx {
inline namespace v1 {
std::unique_ptr<detail::swapchain_implementation> detail::swapchain_implementation::make()
{
    switch (context::current()->options().graphics_api)
    {
    case gapi::opengl: return std::make_unique<opengl::swapchain_implementation>();
    case gapi::vulkan:
        // return std::make_unique<vulkan::swapchain_implementation>();
    default: break;
    }
    return nullptr;
}

uint32_t swapchain::current_image() const noexcept
{
    return implementation()->current_image();
}

void swapchain::present()
{
    implementation()->present();
}

void swapchain::resize(uint32_t width, uint32_t height)
{
    implementation()->resize(width, height);
}
}    // namespace v1
}    // namespace gfx
