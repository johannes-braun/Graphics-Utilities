#include "context_opengl.hpp"

#define MYGL_IMPLEMENTATION
#include <mygl/mygl.hpp>
#include <string_view>

namespace gfx::opengl
{
void context_implementation::initialize(GLFWwindow* window, const context_options& options)
{
    make_current(window);
    mygl::load();
    glDepthFunc(GL_GEQUAL);
    glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
    glClearDepth(0);
    glClearStencil(0);
    glStencilMask(~0);

    int maj, min;
    glGetIntegerv(GL_MAJOR_VERSION, &maj);
    glGetIntegerv(GL_MINOR_VERSION, &min);

    ilog("opengl") << "Initialized an OpenGL " << maj << '.' << min << " context";
    ilog("opengl") << "Vendor: " << glGetString(GL_VENDOR);

    if (options.debug) {
        glDebugMessageCallback(
            [](GLenum source, GLenum type, unsigned int id, GLenum severity, int length, const char* message, const void* userParam) {
                if (id == 131154 || id == 131186) return;

                switch (severity)
                {
                case GL_DEBUG_SEVERITY_MEDIUM: wlog("opengl") << '#' << id << ": " << std::string_view(message, length); break;
                case GL_DEBUG_SEVERITY_HIGH: elog("opengl") << '#' << id << ": " << std::string_view(message, length); break;
                default: break;
                }
            },
            nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW, 0, nullptr, false);
        uint32_t disabled[] = {131154, 131186};
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, std::size(disabled), std::data(disabled), false);
    }
}

void context_implementation::make_current(GLFWwindow* window)
{
    glfwMakeContextCurrent(window);
}
}    // namespace gfx::opengl
