#pragma once

#include <gfx/context.hpp>

namespace gfx::opengl
{
class context_implementation : public detail::context_implementation
{
public:
    void initialize(GLFWwindow* window, const context_options& options) override;
    void make_current(GLFWwindow* window) override;
};
} // namespace gfx::opengl