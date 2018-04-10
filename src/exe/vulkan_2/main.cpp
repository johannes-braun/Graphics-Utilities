#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <thread>
#include <atomic>
#include <queue>
#include <cassert>

#include <vector>
#include <array>
#include <functional>
#include <jpu/log.hpp>

#include "gfx/vk/instance.hpp"
#include "gfx/vk/debug_callback.hpp"
#include "gfx/vk/physical_device.hpp"
#include "gfx/vk/surface.hpp"
#include "gfx/vk/swapchain.hpp"
#include "gfx/vk/semaphore.hpp"
#include "gfx/vk/fence.hpp"
#include "gfx/vk/surface.hpp"
#include "gfx/vk/commands.hpp"

constexpr gfx::vk::app_info default_app_info                { "My App", { 1, 0 }, "My Engine", { 1, 0 } };
constexpr std::array<const char*, 3> layers                 { "VK_LAYER_LUNARG_standard_validation", "VK_LAYER_LUNARG_core_validation", "VK_LAYER_LUNARG_parameter_validation" };
constexpr std::array<const char*, 2> instance_extensions    { "VK_KHR_surface", "VK_EXT_debug_report" };
constexpr std::array<const char*, 2> device_extensions      { "VK_KHR_swapchain", "VK_KHR_push_descriptor" };

constexpr uint32_t queue_index_graphics      = 0;
constexpr uint32_t queue_index_compute       = 1;
constexpr uint32_t queue_index_transfer      = 2;
constexpr uint32_t queue_index_present       = 3;
constexpr std::array<float, 4> queue_priorities = { 0.9f, 1.f, 0.5f, 0.9f };

std::atomic_bool close_window = false;

struct worker {
    worker() = default;
    
    template<typename Fun>
    worker(Fun fun) : _thread(fun) {
        _thread.detach();
    }

    worker(worker&& other) noexcept
    {
        _thread = std::move(other._thread);
        _commands = std::move(other._commands);
    }
    worker& operator=(worker&& other) noexcept
    {
        _thread = std::move(other._thread);
        _commands = std::move(other._commands);
        return *this;
    }

    void consume() {
        std::unique_lock<std::mutex> lock(_command_mutex);
        while (!_commands.empty()) {
            _commands.front()();
            _commands.pop();
        }
    }

    void enqueue(std::function<void()> func)
    {
        std::unique_lock<std::mutex> lock(_command_mutex);
        _commands.push(func);
    }

private:
    std::thread _thread;
    std::queue<std::function<void()>> _commands;
    std::mutex _command_mutex;
};

worker main_thread_worker;

int main()
{
    std::atomic<GLFWwindow*> window = nullptr;
    main_thread_worker = worker([&]() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwSetErrorCallback([](int code, const char* msg) { tlog_e("GLFW Callback") << "[ Code: " << code << " ]: " << msg; });
        window.store(glfwCreateWindow(1280, 720, "Vulkan Sample 2", nullptr, nullptr));

        glfwSetCharCallback(window.load(), [](GLFWwindow* w, uint32_t c) {
            log_i << "Polled";
            if (c == 'x')
                glfwSetWindowShouldClose(w, true);
        });

        while (!close_window)
        {
            auto now = std::chrono::steady_clock::now();
            main_thread_worker.consume();
            glfwPollEvents();
            close_window = glfwWindowShouldClose(window.load());
            while (std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - now).count() < (1/120));
        }

        glfwTerminate();
    });

    while (!window.load());

    uint32_t count = 0; const char** extensions = glfwGetRequiredInstanceExtensions(&count);
    std::vector<const char*> final_instance_extensions(extensions, extensions+count);
    final_instance_extensions.insert(final_instance_extensions.end(), instance_extensions.begin(), instance_extensions.end());
    auto main_instance  = std::make_shared<gfx::vk::instance>(default_app_info, layers, final_instance_extensions);
    auto debug_callback = std::make_shared<gfx::vk::debug_callback>(main_instance, VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT,
        [](VkDebugReportFlagsEXT f, VkDebugReportObjectTypeEXT ot, uint64_t o, size_t l, int32_t m, const char* lp, const char* msg) {
        log_e << msg;
        return false;
    });

    auto main_gpu = [&]() -> std::shared_ptr<gfx::vk::physical_device> {
        for (const std::shared_ptr<gfx::vk::physical_device>& phys_device : main_instance->get_physical_devices())
            return phys_device;
        return nullptr;
    }();

    std::vector<VkQueueFamilyProperties> properties = main_gpu->get_queue_family_properties();
    const auto graphics_queue_iter = std::find_if(properties.begin(), properties.end(), [](const VkQueueFamilyProperties& p) { return (p.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0; });
    const auto compute_queue_iter  = std::find_if(properties.begin(), properties.end(), [](const VkQueueFamilyProperties& p) { return (p.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0; });
    const auto transfer_queue_iter = std::find_if(properties.begin(), properties.end(), [](const VkQueueFamilyProperties& p) { return (p.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0; });
    const auto present_queue_iter  = std::find_if(properties.begin(), properties.end(), [&](const VkQueueFamilyProperties& p) { 
        size_t family = &p - &properties[0];
        return glfwGetPhysicalDevicePresentationSupport(*main_instance, *main_gpu, family);
    });

    struct queue_data { uint32_t family, index; };
    std::unordered_map<uint32_t, std::vector<float>> unique_families;
    std::vector<queue_data> queue_datas;
    
    unique_families[graphics_queue_iter - properties.begin()].push_back(queue_priorities[queue_index_graphics]);
    queue_datas.push_back({ uint32_t(graphics_queue_iter - properties.begin()), uint32_t(unique_families[graphics_queue_iter - properties.begin()].size() - 1) });
    unique_families[compute_queue_iter - properties.begin()].push_back(queue_priorities[queue_index_compute]);
    queue_datas.push_back({ uint32_t(compute_queue_iter - properties.begin()), uint32_t(unique_families[compute_queue_iter - properties.begin()].size() - 1) });
    unique_families[transfer_queue_iter - properties.begin()].push_back(queue_priorities[queue_index_transfer]);
    queue_datas.push_back({ uint32_t(transfer_queue_iter - properties.begin()), uint32_t(unique_families[transfer_queue_iter - properties.begin()].size() - 1) });
    unique_families[present_queue_iter - properties.begin()].push_back(queue_priorities[queue_index_present]);
    queue_datas.push_back({ uint32_t(present_queue_iter - properties.begin()), uint32_t(unique_families[present_queue_iter - properties.begin()].size() - 1) });

    std::vector<gfx::vk::queue_info> queue_infos;
    for (const auto& fam : unique_families) queue_infos.emplace_back(fam.first, fam.second);
    auto device = std::make_shared<gfx::vk::device>(main_gpu, layers, device_extensions, queue_infos, main_gpu->get_features());

    std::vector<gfx::vk::queue> queues;
    for (auto&& data : queue_datas) queues.emplace_back(device, data.family, data.index);

    auto surface            = std::make_shared<gfx::vk::surface>(main_gpu, window.load());
    auto capabilities       = surface->capabilities();
    auto formats            = surface->formats();
    auto present_modes      = surface->present_modes();
    auto swapchain          = surface->create_default_swapchain(device, queues[queue_index_present].family(), 4);
    auto images             = swapchain->images();
    auto command_pool       = std::make_shared<gfx::vk::command_pool>(device, queues[queue_index_graphics].family(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    auto command_buffer     = std::make_shared<gfx::vk::command_buffer>(command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    auto primary_buffers    = command_pool->allocate_buffers(uint32_t(images.size()), VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    auto swap_semaphore     = std::make_shared<gfx::vk::semaphore>(device);
    auto render_semaphore   = std::make_shared<gfx::vk::semaphore>(device);
    
    while (!close_window)
    {
        auto now = std::chrono::steady_clock::now();
        while (std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - now).count() < 1.f);

        main_thread_worker.enqueue([&]() {
            if (glfwGetKey(window.load(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window.load(), true);
        });
        log_i << "Updated";

        uint32_t image = swapchain->next_image(swap_semaphore).second;
        primary_buffers[image]->begin(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

        primary_buffers[image]->end();
        queues[queue_index_present].submit(primary_buffers[image], render_semaphore, swap_semaphore, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
        queues[queue_index_present].present(render_semaphore, swapchain, image);
    }
}