#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <gfx.core/worker.hpp>
#include <gfx.ecs/ecs.hpp>
#include <gfx.graphics/graphics.hpp>
#include <vulkan/vulkan.hpp>

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "VK and GL", nullptr, nullptr);

    gfx::instance  vulkan("Vulkan", {1, 0, 0}, true, true);
    gfx::surface   surface(vulkan, glfwGetWin32Window(window));
    gfx::device    gpu(vulkan, gfx::device_target::gpu, 1.f, {}, surface);
    gfx::swapchain surface_swapchain(gpu, surface);

    gfx::semaphore acquire_semaphore(gpu);

    gfx::ecs::ecs         ecs;
    gfx::ecs::system_list graphics_list;
    gfx::ecs::system_list physics_list;
    gfx::ecs::system_list inputs_list;

    using namespace std::chrono_literals;
    constexpr auto update_time_graphics = 8ms;
    constexpr auto update_time_physics  = 10ms;
    constexpr auto update_time_inputs   = 8ms;

    gfx::worker graphics_worker([&](gfx::worker& self, gfx::worker::duration delta) {
        ecs.update(delta, graphics_list);

        const auto[image, result] = surface_swapchain.next_image(acquire_semaphore);
                
        return self.value_after(!glfwWindowShouldClose(window), update_time_graphics);
    });

    gfx::worker physics_worker([&](gfx::worker& self, gfx::worker::duration delta) {
        ecs.update(delta, physics_list);

        return self.value_after(!glfwWindowShouldClose(window), update_time_physics);
    });

    gfx::worker input_worker([&](gfx::worker& self, gfx::worker::duration delta) {
        ecs.update(delta, inputs_list);

        return self.value_after(!glfwWindowShouldClose(window), update_time_inputs);
    });

    while (!glfwWindowShouldClose(window)) { glfwPollEvents(); }

    glfwDestroyWindow(window);
}
