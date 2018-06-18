#include "context_opengl.hpp"

namespace gfx::opengl
{
void context_implementation::initialize(GLFWwindow* window)
{
    make_current(window);
    mygl::load();
}

void context_implementation::make_current(GLFWwindow* window) { glfwMakeContextCurrent(window); }
} // namespace gfx::opengl
