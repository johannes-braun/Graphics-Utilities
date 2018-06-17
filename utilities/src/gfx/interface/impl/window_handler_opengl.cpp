#include "../../log.hpp"
#include "../../window.hpp"
#include "../window_handler.hpp"
#include <mygl/mygl.hpp>

namespace gfx
{
window_handler_opengl::window_handler_opengl(window& w)
        : window_handler(w)
{
    glfwMakeContextCurrent(w);

    mygl::load();
    glfwSwapInterval(0);

    glDebugMessageCallback(
            [](GLenum         source,
               GLenum         type,
               const uint32_t id,
               const GLenum   severity,
               int /*length*/,
               const char* message,
               const void* user_param) {
                switch(severity)
                {
                case GL_DEBUG_SEVERITY_HIGH:
                    clogc<color_bg(214)>("opengl") << message;
                    break;
                case GL_DEBUG_SEVERITY_MEDIUM:
                    clogc<color_bg(214)>("opengl") << message;
                    break;
                case GL_DEBUG_SEVERITY_LOW:
                    clogc<color_bg(214)>("opengl") << message;
                    break;
                case GL_DEBUG_SEVERITY_NOTIFICATION:
                    clogc<color_bg(214)>("opengl") << message;
                    break;
                default:
                    break;
                }
            },
            nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_MEDIUM, 0, nullptr, false);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW, 0, nullptr, false);

    glDepthFunc(GL_GEQUAL);
    glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
    glClearDepth(0);
    glClearStencil(0);
    glStencilMask(~0);
}

void window_handler_opengl::do_update()
{
    glfwSwapBuffers(get_window());
    glBindFramebuffer(GL_FRAMEBUFFER, mygl::framebuffer::zero);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
} // namespace gfx