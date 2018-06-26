#pragma once

#include <GLFW/glfw3.h>
#include <gfx/graphics/swapchain.hpp>
#include <gfx/context.hpp>

namespace gfx::opengl
{
class swapchain_implementation : public detail::swapchain_implementation
{
public:
    swapchain_implementation();
    uint32_t current_image() const noexcept override;
    void     present()                     override;
    void     resize(uint32_t width, uint32_t height) override;

private:
    std::weak_ptr<context> _context;
    GLFWwindow* _window;
    uint32_t _current_image = 0;
};
} // namespace gfx::vulkan