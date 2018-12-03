#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <gfx.core/worker.hpp>
#include <gfx.ecs/ecs.hpp>
#include <gfx.graphics/graphics.hpp>
#include <vulkan/vulkan.hpp>
#define MYGL_IMPLEMENTATION
#include <mygl/mygl.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "buffer.hpp"
#include "gfx.core/log.hpp"

int main(int argc, char** argv)
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* vulkan_window = glfwCreateWindow(640, 480, "Vulkan", nullptr, nullptr);
    
    gfx::instance  vulkan("Vulkan", {1, 0, 0}, true, true);
    gfx::surface   surface(vulkan, glfwGetWin32Window(vulkan_window));
    gfx::device    gpu(vulkan, gfx::device_target::gpu, 1.f, {}, surface);
    gfx::swapchain surface_swapchain(gpu, surface);
    gfx::semaphore acquire_semaphore(gpu);
    gfx::semaphore finish_semaphore(gpu);
    std::vector<gfx::commands> commands = gpu.allocate_graphics_commands(surface_swapchain.count());
    std::vector<gfx::fence>    command_fences;
    for (size_t i = 0; i < surface_swapchain.count(); ++i)
        command_fences.emplace_back(gpu, true);

    gfx::ecs::ecs         ecs;
    gfx::ecs::system_list vulkan_graphics_list;
    gfx::ecs::system_list opengl_graphics_list;
    gfx::ecs::system_list physics_list;
    gfx::ecs::system_list inputs_list;

    using namespace std::chrono_literals;
    constexpr auto update_time_graphics = 0ms;
    constexpr auto update_time_physics  = 10ms;
    constexpr auto update_time_inputs   = 8ms;
    
    gfx::worker::duration vulkan_combined_delta = 0s;
    int vulkan_frames = 0;
    gfx::worker vulkan_graphics_worker([&](gfx::worker& self, gfx::worker::duration delta) {
        vulkan_combined_delta += delta;
        ++vulkan_frames;
        if(vulkan_combined_delta > 1s)
        {
            gfx::ilog("VULKAN") << std::to_string(vulkan_frames / vulkan_combined_delta.count()) << "fps";
            vulkan_frames = 0;
            vulkan_combined_delta = 0s;
        }

        ecs.update(delta, vulkan_graphics_list);

        if (surface_swapchain.swap(acquire_semaphore))
        {
            if (!surface_swapchain.recreate()) 
                return false;
            return true;
        }

        const auto& current = commands[surface_swapchain.current_index()];

        gpu.wait_for({ command_fences[surface_swapchain.current_index()] });
        gpu.reset_fences({ command_fences[surface_swapchain.current_index()] });

        current.get_command_buffer().reset({});
        current.get_command_buffer().begin(vk::CommandBufferBeginInfo{vk::CommandBufferUsageFlagBits::eSimultaneousUse});
        vk::ImageMemoryBarrier make_presentable;
        make_presentable.image         = surface_swapchain.current_image().get_image();
        make_presentable.oldLayout     = vk::ImageLayout::eUndefined;
        make_presentable.newLayout     = vk::ImageLayout::eTransferDstOptimal;
        make_presentable.srcAccessMask = {};
        make_presentable.dstAccessMask = vk::AccessFlagBits::eMemoryWrite;
        current.get_command_buffer().pipelineBarrier(vk::PipelineStageFlagBits::eBottomOfPipe, vk::PipelineStageFlagBits::eTransfer,
                                                     vk::DependencyFlagBits::eByRegion, {}, {}, make_presentable);
        current.get_command_buffer().clearColorImage(surface_swapchain.current_image().get_image(), vk::ImageLayout::eTransferDstOptimal,
                                                     vk::ClearColorValue(std::array{0.8f, 0.1f, 0.f, 1.f}),
                                                     vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
        make_presentable.oldLayout     = vk::ImageLayout::eTransferDstOptimal;
        make_presentable.newLayout     = vk::ImageLayout::ePresentSrcKHR;
        make_presentable.srcAccessMask = vk::AccessFlagBits::eMemoryWrite;
        make_presentable.dstAccessMask = vk::AccessFlagBits::eMemoryRead;
        current.get_command_buffer().pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTopOfPipe,
                                                     vk::DependencyFlagBits::eByRegion, {}, {}, make_presentable);
        current.get_command_buffer().end();
        gpu.graphics_queue().submit({current}, {acquire_semaphore}, {finish_semaphore}, command_fences[surface_swapchain.current_index()]);

        if (gpu.present_queue().present({{surface_swapchain.current_index(), surface_swapchain}}, {finish_semaphore}))
        {
            if (!surface_swapchain.recreate()) 
                return false;
            return true;
        }

        return self.value_after(true, update_time_graphics);
    });

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    GLFWwindow* opengl_window = glfwCreateWindow(640, 480, "OpenGL", nullptr, nullptr);
    glfwMakeContextCurrent(opengl_window);
    mygl::dispatch gl(reinterpret_cast<mygl::loader_function>(&glfwGetProcAddress));
    glfwMakeContextCurrent(nullptr);
    std::atomic_bool init = false;
        
    gfx::worker::duration opengl_combined_delta = 0s;
    int opengl_frames = 0;
    gfx::worker opengl_graphics_worker([&](gfx::worker& self, gfx::worker::duration delta)
    {
        opengl_combined_delta += delta;
        ++opengl_frames;
        if (opengl_combined_delta > 1s)
        {
            gfx::ilog("OPENGL") << std::to_string(opengl_frames / opengl_combined_delta.count()) << "fps";
            opengl_frames = 0;
            opengl_combined_delta = 0s;
        }
        ecs.update(delta, opengl_graphics_list);
        if(!init.exchange(true))
            glfwMakeContextCurrent(opengl_window);
        glm::vec4 clear_color(0.3f, 0.5f, 0.9f, 1.f);
        gl.clearNamedFramebufferfv(mygl::framebuffer::zero(), GL_COLOR, 0, glm::value_ptr(clear_color));
        glfwSwapBuffers(opengl_window);
        return self.value_after(true, update_time_graphics);
    });

    gfx::worker physics_worker([&](gfx::worker& self, gfx::worker::duration delta) {
        ecs.update(delta, physics_list);
        return self.value_after(true, update_time_physics);
    });

    gfx::worker input_worker([&](gfx::worker& self, gfx::worker::duration delta) {
        ecs.update(delta, inputs_list);
        return self.value_after(true, update_time_inputs);
    });

    while (!init);
    while(!glfwWindowShouldClose(vulkan_window) && !glfwWindowShouldClose(opengl_window))
        glfwPollEvents();
}
