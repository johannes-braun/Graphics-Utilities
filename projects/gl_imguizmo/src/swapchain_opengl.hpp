#pragma once

#include <GLFW/glfw3.h>
#include "swapchain.hpp"

namespace gfx::opengl
{
class swapchain_implementation : public detail::swapchain_implementation
{
public:
    swapchain_implementation();
    uint32_t current_image() const noexcept override;
    void     present()                     override;

private:
    GLFWwindow* _window;
    uint32_t _current_image = 0;
};
} // namespace gfx::vulkan