#define MYGL_IMPLEMENTATION
#include <GLFW/glfw3.h>
#include <mygl/mygl.hpp>
#include <thread>
#define GLFW_EXPOSE_NATIVE_WIN32
#include "gfx.core/log.hpp"
#include "globals.hpp"
#include <GLFW/glfw3native.h>
#include <gfx.core/worker.hpp>
#include <gfx.ecs/ecs.hpp>
#include <gfx.graphics/graphics.hpp>
#include <glm/ext.hpp>
#include <glm/glm.hpp>

namespace impl::opengl {
ecs_state_t* _current_state;
void         run();
}    // namespace impl::gl
std::thread run_opengl(ecs_state_t& ecs_state)
{
    impl::opengl::_current_state = &ecs_state;
    return std::thread([] { impl::opengl::run(); });
}
namespace impl::opengl {
void run()
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    GLFWwindow* opengl_window = glfwCreateWindow(800, 800, "OpenGL", nullptr, nullptr);
    glfwMakeContextCurrent(opengl_window);
    mygl::dispatch gl(reinterpret_cast<mygl::loader_function>(&glfwGetProcAddress));
    glfwMakeContextCurrent(nullptr);
    std::atomic_bool      init = false;
    gfx::ecs::system_list graphics_list;

    glfwSwapInterval(0);
    gfx::worker::duration opengl_combined_delta = 0s;
    int                   opengl_frames         = 0;
    gfx::worker           opengl_graphics_worker([&](gfx::worker& self, gfx::worker::duration delta) {
        opengl_combined_delta += delta;
        ++opengl_frames;
        if (opengl_combined_delta > 1s)
        {
            gfx::ilog("OPENGL") << std::to_string(opengl_frames / opengl_combined_delta.count()) << "fps";
            opengl_frames         = 0;
            opengl_combined_delta = 0s;
        }
        _current_state->ecs.update(delta, graphics_list);
        if (!init.exchange(true)) glfwMakeContextCurrent(opengl_window);
        glm::vec4 clear_color(0.3f, 0.5f, 0.9f, 1.f);
        gl.clearNamedFramebufferfv(mygl::framebuffer::zero(), GL_COLOR, 0, glm::value_ptr(clear_color));
        glfwSwapBuffers(opengl_window);
        return self.value_after(true, update_time_graphics);
    });

    while (!glfwWindowShouldClose(opengl_window)) glfwPollEvents();
}
}    // namespace impl::gl