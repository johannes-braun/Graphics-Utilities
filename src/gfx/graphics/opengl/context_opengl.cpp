#include "context_opengl.hpp"

#define MYGL_IMPLEMENTATION
#include <mygl/mygl.hpp>

namespace gfx::opengl
{
void context_implementation::initialize(GLFWwindow* window)
{

    make_current(window);
    mygl::load();
    glDepthFunc(GL_GEQUAL);
    glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
    glClearDepth(0);
    glClearStencil(0);
    glStencilMask(~0);
}

void context_implementation::make_current(GLFWwindow* window) { glfwMakeContextCurrent(window); }
} // namespace gfx::opengl
