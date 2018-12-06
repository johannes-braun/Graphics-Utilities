#include <GLFW/glfw3.h>
#include <gfx.core/worker.hpp>
#include <gfx.ecs/ecs.hpp>
#include <thread>

#include "globals.hpp"

std::thread run_vulkan(ecs_state_t& ecs_state);
std::thread run_opengl(ecs_state_t& ecs_state);

int main(int argc, char** argv)
{
    struct glfw
    {
        glfw() { glfwInit(); }
        ~glfw() { glfwTerminate(); }
    } init;

    ecs_state_t ecs_state;

    std::vector<std::thread> wait_for;
    wait_for.emplace_back(run_vulkan(ecs_state));
    wait_for.emplace_back(run_opengl(ecs_state));

    gfx::worker physics_worker([&](gfx::worker& self, gfx::worker::duration delta) {
        ecs_state.ecs.update(delta, ecs_state.physics_list);
        return self.value_after(true, update_time_physics);
    });

    gfx::worker input_worker([&](gfx::worker& self, gfx::worker::duration delta) {
        ecs_state.ecs.update(delta, ecs_state.inputs_list);
        return self.value_after(true, update_time_inputs);
    });

    for (auto& t : wait_for)
        if (t.joinable()) t.join();
}
