#pragma once

#include "gfx/alias.inl"
#include "gfx/graphics/framebuffer.hpp"
#include <GLFW/glfw3.h>
#include <gfx/context.hpp>
#include <gfx/graphics/swapchain.hpp>

namespace gfx {
inline namespace v1 {
namespace opengl {
class swapchain_implementation : public detail::swapchain_implementation
{
public:
    swapchain_implementation();
    uint32_t                       current_image() const noexcept override;
    void                           present() override;
    void                           resize(uint32_t width, uint32_t height) override;
    std::any                       api_handle() override;
    const std::vector<image_view>& image_views() const override;

private:
    std::vector<image>       _images;
    std::vector<image_view>  _image_views;
    std::weak_ptr<context>   _context;
    std::vector<framebuffer> _blit_helpers;
    GLFWwindow*              _window;
    uint32_t                 _current_image = 0;
    uint32_t                 _image_count   = 0;
};
}    // namespace opengl
}    // namespace v1
}    // namespace gfx