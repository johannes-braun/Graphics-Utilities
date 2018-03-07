#include "window.hpp"

#include <stb_image.h>
#include <vulkan/detail.hpp>
#include <mygl/gl.hpp>
#include <opengl/debug.hpp>

namespace io
{
    window::window(const api api, const int width, const int height, const std::string_view title, const std::optional<monitor> monitor) : window(api,
        width, height, title, nullptr, monitor)
    {}

    window::window(api api, const int width, const int height, std::string_view title, window* share, std::optional<monitor> monitor)
        : _api(api)
    {
        glfwWindowHint(GLFW_CLIENT_API, static_cast<int>(api));
        glfwSetErrorCallback([](int code, const char* msg) { tlog_e("GLFW Callback") << "[ Code: " << code << " ]: " << msg; });
        _window = glfwCreateWindow(width, height, title.data(),
            monitor.has_value() ? static_cast<GLFWmonitor*>(*monitor) : nullptr,
            share ? static_cast<GLFWwindow*>(*share) : nullptr);
        callbacks = std::make_unique<io::callbacks>(_window);
        callbacks->key_callback.add([this](GLFWwindow*, int key, int, int action, int mods) {
            gui()->key_action(key, action, mods);
        });
        callbacks->scroll_callback.add([this](GLFWwindow*, double x, double y) {
            gui()->scrolled(y);
        });
        callbacks->char_callback.add([this](GLFWwindow*, unsigned c) {
            gui()->char_input(static_cast<wchar_t>(c));
        });
        callbacks->mouse_button_callback.add([this](GLFWwindow*, int key, int action, int mods) {
            gui()->mouse_button_action(key, action, mods);
        });

        if (_api == api::vulkan)
        {
            vk::ApplicationInfo application_info(title.data(), VK_MAKE_VERSION(1, 0, 0), title.data(), VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0);
            auto extensions = vkn::detail::instance_extensions();
            auto layers = vkn::detail::instance_layers();

            _instance = createInstance(vk::InstanceCreateInfo({}, &application_info,
                static_cast<uint32_t>(layers.size()), layers.data(),
                static_cast<uint32_t>(extensions.size()), extensions.data()));
            if (!_instance) throw std::runtime_error("Could not create instance.");

            vkExtInitInstance(static_cast<VkInstance>(_instance));

            _debug_callback_fun = [](const vk::DebugReportFlagBitsEXT flags, vk::DebugReportObjectTypeEXT obj_type,
                uint64_t obj, size_t location, int32_t code, std::string_view layer_prefix, const std::string_view msg) {
                switch (flags)
                {
                case vk::DebugReportFlagBitsEXT::eError:
                    tlog_e("Vulkan Debug") << msg; break;
                case vk::DebugReportFlagBitsEXT::eWarning:
                    tlog_w("Vulkan Debug") << msg; break;
                case vk::DebugReportFlagBitsEXT::ePerformanceWarning:
                    tlog_h("Vulkan Debug") << msg; break;
                case vk::DebugReportFlagBitsEXT::eInformation:
                    tlog_i("Vulkan Debug") << msg; break;
                case vk::DebugReportFlagBitsEXT::eDebug:
                    tlog_d("Vulkan Debug") << msg; break;
                }

                return false;
            };
            _debug_callback = _instance.createDebugReportCallbackEXT(vk::DebugReportCallbackCreateInfoEXT(
                vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning | vk::DebugReportFlagBitsEXT::ePerformanceWarning, &debugCallback, &_debug_callback_fun));
            if (!(_debug_callback_fun && _debug_callback)) throw std::runtime_error("Could not create debug callback.");

            glfwCreateWindowSurface(static_cast<VkInstance>(_instance), _window, nullptr, reinterpret_cast<VkSurfaceKHR*>(&_surface));
            if (!_surface) throw std::runtime_error("Could not create surface.");

            log_i << "Vulkan Window creation finished successfully";

            switch (auto available = _instance.enumeratePhysicalDevices(); available.size())
            {
            case 0:
                throw std::runtime_error("Cannot fetch suitable Physical Device");
            default:
                _physical_device = available[0];
            }

            _device = jpu::make_ref<vkn::device>(_physical_device, _surface,
                vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eTransfer | vk::QueueFlagBits::eCompute, 1ui32 << 25ui32);
            _swapchain = jpu::make_ref<vkn::swapchain>(_device.get(), _surface, 8);

            _gui = jpu::make_ref<io::gui>(_window, _device, _swapchain);

            _primary_command_buffers = _device->allocateCommandBuffers(vk::CommandBufferAllocateInfo(
                _device->command_pool(vk::QueueFlagBits::eGraphics),
                vk::CommandBufferLevel::ePrimary,
                static_cast<uint32_t>(_swapchain->images().size())));
            _memory_fence = _device->createFence({ vk::FenceCreateFlagBits::eSignaled });
            _semaphore_render_finished = _device->createSemaphore({});
        }
        else if (_api == api::opengl)
        {
            glfwMakeContextCurrent(_window);

            mygl_load_gl();
            glfwSwapInterval(0);

            gl::set_debug_callback([](gl::debug_source source, gl::debug_type type, uint32_t id,
                gl::debug_severity severity, const char* message) {
                switch (severity)
                {
                case gl::debug_severity::high:
                    tlog_e("OpenGL Debug") << "source=\"" << get_debug_enum_desc(source) << "\", type=\"" <<
                        get_debug_enum_desc(type) << "\" -- " << message;
                    break;
                case gl::debug_severity::medium:
                    tlog_w("OpenGL Debug") << "source=\"" << get_debug_enum_desc(source) << "\", type=\"" <<
                        get_debug_enum_desc(type) << "\" -- " << message;
                    break;
                case gl::debug_severity::low:
                    tlog_d("OpenGL Debug") << "source=\"" << get_debug_enum_desc(source) << "\", type=\"" <<
                        get_debug_enum_desc(type) << "\" -- " << message;
                    break;
                case gl::debug_severity::notification:
                    tlog_v("OpenGL Debug") << "source=\"" << get_debug_enum_desc(source) << "\", type=\"" <<
                        get_debug_enum_desc(type) << "\" -- " << message;
                    break;
                default: break;
                }
            });
            set_debug_callback_enabled(gl::debug_severity::notification, false);
            set_debug_callback_enabled(gl::debug_severity::low, false);
            _gui = jpu::make_ref<io::gui>(_window);
            glDepthFunc(GL_GEQUAL);
            glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
            glClearDepth(0);
        }
    }

    window::~window()
    {
        _gui.reset();
        switch (_api)
        {
        case api::vulkan:
            _swapchain.reset();
            _device->destroyFence(_memory_fence);
            _device->destroySemaphore(_semaphore_render_finished);
            _instance.destroyDebugReportCallbackEXT(_debug_callback);
            _instance.destroySurfaceKHR(_surface);
            _instance.destroy();
            break;
        case api::opengl: break;
        default: break;
        };

        glfwDestroyWindow(_window);
    }

    gui* window::gui() const
    {
        return _gui.get();
    }

    bool window::update()
    {
        if (_api == api::vulkan)
            _gui->render_interface_vk().set_next_command_buffer(_current_primary_command_buffer);

        if (_gui->is_initialized())
            _gui->render();
        else
            glfwSetTime(0);

        if (_api == api::vulkan)
        {
            if (_current_primary_command_buffer)
            {
                _current_primary_command_buffer.end();

                // Submit primary command buffer for rendering
                std::array<vk::CommandBuffer, 1> command_buffers{ _current_primary_command_buffer };
                const auto wait_semaphores = { _swapchain->swap_semaphore() };

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

                vk::SwapchainKHR sc = *_swapchain;
                const auto present_semaphores = { _semaphore_render_finished };
                const auto image = { _swapchain->current_image() };
                _device->queue(vk::QueueFlagBits::eGraphics).queue.presentKHR(vk::PresentInfoKHR(static_cast<uint32_t>(std::size(present_semaphores)), std::data(present_semaphores), 1, &sc, std::data(image)));
            }
        }
        glfwPollEvents();

        int ww, wh;
        glfwGetFramebufferSize(_window, &ww, &wh);
        while (ww*wh == 0)
        {
            glfwPollEvents();
            glfwGetFramebufferSize(_window, &ww, &wh);
        }

        if (_api == api::opengl)
        {
            glfwSwapBuffers(_window);
            glBindFramebuffer(GL_FRAMEBUFFER, gl_framebuffer_t(0));
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
        else if (_api == api::vulkan)
        {
            _swapchain->swap();

            _device->waitForFences(_memory_fence, true, std::numeric_limits<uint64_t>::max());
            _device->resetFences(_memory_fence);

            _current_primary_command_buffer = _primary_command_buffers[_swapchain->current_image()];
            _current_primary_command_buffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eSimultaneousUse));
        }

        while (_last_time > glfwGetTime() - _swap_delay)
            glfwPollEvents();

        const auto is_open = !glfwWindowShouldClose(_window);
        if (is_open) _gui->new_frame();
        _delta_time = (glfwGetTime() - _last_time);
        _last_time = glfwGetTime();
        return is_open;
    }

    window::operator GLFWwindow*() const
    {
        return _window;
    }

    void window::set_icon(const int w, const int h, const void* data) const
    {
        struct img {
            int width;
            int height;
            const unsigned char* pixels;
        } icon_image;
        icon_image.width = w;
        icon_image.height = h;
        icon_image.pixels = static_cast<const unsigned char*>(data);
        glfwSetWindowIcon(_window, 1, reinterpret_cast<const GLFWimage*>(&icon_image));
    }

    void window::set_monitor(std::optional<monitor> monitor)
    {
        if (monitor)
        {
            glfwGetFramebufferSize(_window, &_width_before_monitor, &_height_before_monitor);
            glfwGetWindowPos(_window, &_position_before_monitor_x, &_position_before_monitor_y);
            const GLFWvidmode* mode = glfwGetVideoMode(*monitor);
            glfwSetWindowMonitor(_window, *monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        }
        else if (glfwGetWindowMonitor(_window) != *monitor)
        {
            glfwSetWindowMonitor(_window, nullptr, _position_before_monitor_x, _position_before_monitor_y, _width_before_monitor, _height_before_monitor, GLFW_DONT_CARE);
        }
    }

    double window::delta_time() const
    {
        return _delta_time;
    }

    void window::close() const
    {
        glfwSetWindowShouldClose(_window, true);
    }

    glfw_pre_init::glfw_pre_init()
    {
        glfwInit();
    }

    glfw_pre_init::~glfw_pre_init()
    {
        glfwTerminate();
    }

    monitor::monitor() : monitor(0)
    {}

    monitor::monitor(const int index)
    {
        int count;
        _monitor = glfwGetMonitors(&count)[index];
        _mode = glfwGetVideoMode(_monitor);
    }

    const GLFWvidmode& monitor::video_mode() const
    {
        return *_mode;
    }

    monitor::operator GLFWmonitor*() const
    {
        return _monitor;
    }
}
