#pragma once

#include <mygl/mygl.hpp>
#include "context.hpp"

namespace gfx::opengl
{
class context_implementation : public detail::context_implementation
{
public:
    void initialize(GLFWwindow* window) override;
    void make_current(GLFWwindow* window) override;
};
} // namespace gfx::opengl