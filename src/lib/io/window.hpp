#pragma once

#if defined(IO_API_VULKAN)
#include "vulkan/vulkan.hpp"
#include "vulkan/detail.hpp"
#include "vulkan/logical_device.hpp"
#include "vulkan/swapchain.hpp"
#include "vulkan/vulkan_ext.h"
#elif defined(IO_API_OPENGL)
#include "glad/glad.h"
#include "opengl/debug.hpp"
#endif
#include "GLFW/glfw3.h"
#include <optional>
#include <jpu/memory>
#include <filesystem>
#include "gui.hpp"
#include <functional>
#include <jpu/log>

namespace io
{
    static struct glfw_pre_init
    {
        glfw_pre_init();
        ~glfw_pre_init();
    } init;

    class monitor
    {
    public:
        monitor();
        monitor(int index);
        const GLFWvidmode& video_mode() const;
        operator GLFWmonitor*() const;

    private:
        GLFWmonitor * _monitor;
        const GLFWvidmode* _mode;
    };

    class window : public jpu::ref_count
    {
    public:
        window(int width, int height, std::string_view title, std::optional<monitor> monitor = {}) : window(
            width, height, title, nullptr, monitor)
        {}

        window(int width, int height, std::string_view title, window* share, std::optional<monitor> monitor = {})
        {
#if defined(IO_API_VULKAN)
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif
            _window = glfwCreateWindow(width, height, title.data(),
                monitor.has_value() ? static_cast<GLFWmonitor*>(*monitor) : nullptr,
                share ? static_cast<GLFWwindow*>(*share) : nullptr);
            glfwMakeContextCurrent(_window);

#if defined(IO_API_VULKAN)
            vk::ApplicationInfo application_info(title.data(), VK_MAKE_VERSION(1, 0, 0), title.data(), VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0);
            auto extensions = vkn::detail::instance_extensions();
            auto layers = vkn::detail::instance_layers();

            _instance = vk::createInstance(vk::InstanceCreateInfo({}, &application_info,
#if defined(NDEBUG)
                0, nullptr,
#else
                static_cast<uint32_t>(layers.size()), layers.data(),
#endif
                static_cast<uint32_t>(extensions.size()), extensions.data()));
            if (!_instance) throw std::runtime_error("Could not create instance.");

            vkExtInitInstance(static_cast<VkInstance>(_instance));

            _debug_callback_fun = [](const vk::DebugReportFlagBitsEXT flags, vk::DebugReportObjectTypeEXT obj_type,
                uint64_t obj, size_t location, int32_t code, std::string_view layer_prefix, const std::string_view msg) {
                switch (flags)
                {
                case vk::DebugReportFlagBitsEXT::eError:
                    log_e << msg; break;
                case vk::DebugReportFlagBitsEXT::eWarning:
                    log_w << msg; break;
                case vk::DebugReportFlagBitsEXT::ePerformanceWarning:
                    log_h << msg; break;
                case vk::DebugReportFlagBitsEXT::eInformation:
                    log_i << msg; break;
                case vk::DebugReportFlagBitsEXT::eDebug:
                    log_d << msg; break;
                }

                return false;
            };
            _debug_callback = _instance.createDebugReportCallbackEXT(vk::DebugReportCallbackCreateInfoEXT(
                vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning | vk::DebugReportFlagBitsEXT::ePerformanceWarning, &debugCallback, &_debug_callback_fun));
            if (!(_debug_callback_fun && _debug_callback)) throw std::runtime_error("Could not create debug callback.");

            glfwCreateWindowSurface(static_cast<VkInstance>(_instance), _window, nullptr, reinterpret_cast<VkSurfaceKHR*>(&_surface));
            if (!_surface) throw std::runtime_error("Could not create surface.");

            log_i << "Vulkan Window creation finished successfully";

            _physical_device = vkn::devices::findPhysical(_instance);

            _device = jpu::make_ref<vkn::LogicalDevice>(vkn::LogicalDeviceCreateInfo(_physical_device, _surface,
                vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eTransfer | vk::QueueFlagBits::eCompute, 1ui32 << 25ui32));
            _swapchain = jpu::make_ref<vkn::Swapchain>(vkn::SwapchainCreateInfo(_surface, _device, 8));

            _gui = jpu::make_ref<io::gui>(_window, _device, _swapchain);

            _primary_command_buffers = _device->allocateCommandBuffers(vk::CommandBufferAllocateInfo(
                _device->commandPool(vk::QueueFlagBits::eGraphics),
                vk::CommandBufferLevel::ePrimary,
                static_cast<uint32_t>(_swapchain->images().size())));
            _memory_fence = _device->createFence({ vk::FenceCreateFlagBits::eSignaled });
            _semaphore_render_finished = _device->createSemaphore({});
#elif defined(IO_API_OPENGL)
            gladLoadGL();

            gl::set_debug_callback([](gl::debug_source source, gl::debug_type type, uint32_t id, gl::debug_severity severity, const char* message) {
                switch (severity)
                {
                case gl::debug_severity::high:
                    log_e << "source=\"" << gl::get_debug_enum_desc(source) << "\", type=\"" << gl::get_debug_enum_desc(type) << "\" -- " << message;
                    break;
                case gl::debug_severity::medium:
                    log_w << "source=\"" << gl::get_debug_enum_desc(source) << "\", type=\"" << gl::get_debug_enum_desc(type) << "\" -- " << message;
                    break;
                case gl::debug_severity::low:
                    log_d << "source=\"" << gl::get_debug_enum_desc(source) << "\", type=\"" << gl::get_debug_enum_desc(type) << "\" -- " << message;
                    break;
                case gl::debug_severity::notification:
                    log_v << "source=\"" << gl::get_debug_enum_desc(source) << "\", type=\"" << gl::get_debug_enum_desc(type) << "\" -- " << message;
                    break;
                default: break;
                }
            });
            gl::set_debug_callback_enabled(gl::debug_severity::notification, false);
            gl::set_debug_callback_enabled(gl::debug_severity::low, false);
            _gui = jpu::make_ref<io::gui>(_window);
#endif
        }

        ~window()
        {
            _gui.reset();
#ifdef IO_API_VULKAN
            _device->destroyFence(_memory_fence);
            _device->destroySemaphore(_semaphore_render_finished);
            _instance.destroyDebugReportCallbackEXT(_debug_callback);
            _instance.destroySurfaceKHR(_surface);
            _instance.destroy();
#endif
            glfwDestroyWindow(_window);
        }

        operator GLFWwindow*() const;
        gui* gui() const
        {
            return _gui.get();
        }

        void load_icon(const std::experimental::filesystem::path& path);
        void set_monitor(std::optional<monitor> monitor);


        bool update()
        {
#if defined(IO_API_VULKAN)
            _gui->render_interface().set_next_command_buffer(_current_primary_command_buffer);
#endif
            if (_gui->is_initialized())
                _gui->render();
#if defined(IO_API_VULKAN)
            if (_current_primary_command_buffer)
            {
                _current_primary_command_buffer.end();

                // Submit primary command buffer for rendering
                vk::ArrayProxy<const vk::CommandBuffer> command_buffers{ _current_primary_command_buffer };
                const auto wait_semaphores = { _swapchain->imageSemaphore() };

                vk::PipelineStageFlags stage_flags = vk::PipelineStageFlagBits::eColorAttachmentOutput;
                vk::SubmitInfo render_submit_info;
                render_submit_info.setCommandBufferCount(static_cast<uint32_t>(std::size(command_buffers)))
                    .setPCommandBuffers(std::data(command_buffers))
                    .setSignalSemaphoreCount(1)
                    .setPSignalSemaphores(&_semaphore_render_finished)
                    .setWaitSemaphoreCount(static_cast<uint32_t>(std::size(wait_semaphores)))
                    .setPWaitSemaphores(std::data(wait_semaphores))
                    .setPWaitDstStageMask(&stage_flags);
                _device->queue(vk::QueueFlagBits::eGraphics).queue.submit(render_submit_info, _memory_fence);

                const auto sc = _swapchain->swapchain();
                const auto present_semaphores = { _semaphore_render_finished };
                const auto image = { _swapchain->currentImage() };
                _device->queue(vk::QueueFlagBits::eGraphics).queue.presentKHR(vk::PresentInfoKHR(static_cast<uint32_t>(std::size(present_semaphores)), std::data(present_semaphores), 1, &sc, std::data(image)));
            }
#endif
            glfwPollEvents();

#if defined(IO_API_OPENGL)
            glfwSwapBuffers(_window);
#elif defined(IO_API_VULKAN)
            while (!_swapchain->visible())
                glfwPollEvents();
            _swapchain->swap();

            _device->waitForFences(_memory_fence, true, std::numeric_limits<uint64_t>::max());
            _device->resetFences(_memory_fence);

            _current_primary_command_buffer = _primary_command_buffers[_swapchain->currentImage()];
            _current_primary_command_buffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eSimultaneousUse));
#endif
            const auto is_open = !glfwWindowShouldClose(_window);
            if (is_open) _gui->new_frame();
            _delta_time = (glfwGetTime() - _last_time);
            _last_time = glfwGetTime();
            return is_open;
        }

        double delta_time() const;
        void close() const;

#if defined(IO_API_VULKAN)
        using debug_callback = std::function<bool(vk::DebugReportFlagBitsEXT flags, vk::DebugReportObjectTypeEXT type, uint64_t object, size_t location, int32_t code, std::string_view layer_prefix, std::string_view message)>;

#ifdef IO_EXPOSE_API_FUNCTIONS
        vk::Instance instance() const { return _instance; }
        vk::SurfaceKHR surface() const { return _surface; }
        vk::PhysicalDevice gpu() const { return _physical_device; }
        vkn::LogicalDevice* device() const { return _device.get(); }
        vkn::Swapchain* swapchain() const { return _swapchain.get(); }
        const std::vector<vk::CommandBuffer>& primary_command_buffers() const { return _primary_command_buffers; }
        vk::CommandBuffer current_primary_command_buffer() const { return _current_primary_command_buffer; }
        void rebuild_swapchain()
        {
            _device->waitIdle();
            while (_swapchain->dec_ref());
            _swapchain.reset();
            _swapchain = jpu::make_ref<vkn::Swapchain>(vkn::SwapchainCreateInfo(_surface, _device.get(), 8));
            _gui->render_interface().update_swapchain(_swapchain.get());
        }
#endif
#endif

    private:
        jpu::ref_ptr<io::gui> _gui;
        struct freer { void operator()(unsigned char* d) const { free(d); } };
        std::unique_ptr<unsigned char[], freer> _icon_storage;
        GLFWimage _icon_image;
        int _width_before_monitor{ 0 };
        int _height_before_monitor{ 0 };
        int _position_before_monitor_x{ 0 };
        int _position_before_monitor_y{ 0 };
        double _delta_time{ 0 };
        double _last_time{ 0 };
        GLFWwindow * _window{ nullptr };

#if defined(IO_API_VULKAN)
        vk::SurfaceKHR _surface;
        vk::Instance _instance;
        vk::PhysicalDevice _physical_device;
        jpu::ref_ptr<vkn::LogicalDevice> _device;
        jpu::ref_ptr<vkn::Swapchain> _swapchain;
        std::vector<vk::CommandBuffer> _primary_command_buffers;
        vk::CommandBuffer _current_primary_command_buffer;
        vk::Semaphore _semaphore_render_finished;
        vk::Fence _memory_fence;

        debug_callback _debug_callback_fun;
        vk::DebugReportCallbackEXT _debug_callback;

        VKAPI_ATTR static vk::Bool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT obj_type,
            uint64_t obj,
            size_t location, int32_t code, const char* layer_prefix,
            const char* msg, void* user_data)
        {
            return user_data ? (*reinterpret_cast<window::debug_callback*>(user_data))(static_cast<vk::DebugReportFlagBitsEXT>(flags),
                static_cast<vk::DebugReportObjectTypeEXT>(obj_type),
                obj, location, code, layer_prefix, msg) : false;
        }
#endif
        };
        }
