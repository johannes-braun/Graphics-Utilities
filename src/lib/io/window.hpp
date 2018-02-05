#pragma once

#if defined(IO_API_VULKAN)
#include "vulkan/vulkan.hpp"
#include "vulkan/detail.hpp"
#include "vulkan/device.hpp"
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
#include "gui_impl/gui.hpp"
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
        window(int width, int height, std::string_view title, std::optional<monitor> monitor = {});
        window(int width, int height, std::string_view title, window* share, std::optional<monitor> monitor = {});
        ~window();

        operator GLFWwindow*() const;
        gui* gui() const;

        void load_icon(const std::experimental::filesystem::path& path);
        void set_monitor(std::optional<monitor> monitor);

        bool update();

        double delta_time() const;
        void close() const;

#if defined(IO_API_VULKAN)
        using debug_callback = std::function<bool(vk::DebugReportFlagBitsEXT flags, vk::DebugReportObjectTypeEXT type, uint64_t object, size_t location, int32_t code, std::string_view layer_prefix, std::string_view message)>;

#ifdef IO_EXPOSE_API_FUNCTIONS
        vk::Instance instance() const { return _instance; }
        vk::SurfaceKHR surface() const { return _surface; }
        vk::PhysicalDevice gpu() const { return _physical_device; }
        vkn::device* device() const { return _device.get(); }
        vkn::swapchain* swapchain() const { return _swapchain.get(); }
        const std::vector<vk::CommandBuffer>& primary_command_buffers() const { return _primary_command_buffers; }
        vk::CommandBuffer current_primary_command_buffer() const { return _current_primary_command_buffer; }
        void rebuild_swapchain()
        {
            _device->waitIdle();
            while (_swapchain->dec_ref() > 1);
            _swapchain.reset();
            _swapchain = jpu::make_ref<vkn::swapchain>(_device.get(), _surface, 8);
            _gui->render_interface().update_swapchain(_swapchain.get());

            for (auto && cmdbuf : _primary_command_buffers)
                cmdbuf.reset({});

            if (_current_primary_command_buffer)
            {
                _swapchain->swap();
                _current_primary_command_buffer = _primary_command_buffers[_swapchain->current_image()];
                _current_primary_command_buffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eSimultaneousUse));
            }
        }
#endif
#endif

    private:
        jpu::ref_ptr<io::gui> _gui;
        struct freer {
            void operator()(unsigned char* d) const;
        };
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
        jpu::ref_ptr<vkn::device> _device;
        jpu::ref_ptr<vkn::swapchain> _swapchain;
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


#include "window.inl"