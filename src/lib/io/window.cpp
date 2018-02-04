#include "window.hpp"
#include "stb/stb_image.h"

namespace io
{
    window::operator struct GLFWwindow*() const
    {
        return _window;
    }

    void window::load_icon(const std::experimental::filesystem::path& path)
    {
        _icon_storage = decltype(_icon_storage)(stbi_load(path.string().c_str(), &_icon_image.width, &_icon_image.height, nullptr, STBI_rgb_alpha));
        _icon_image.pixels = _icon_storage.get();
        glfwSetWindowIcon(_window, 1, &_icon_image);
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
        else
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

    monitor::monitor(int index)
    {
        int count;
        _monitor = glfwGetMonitors(&count)[index];
        _mode = glfwGetVideoMode(_monitor);
    }

    const GLFWvidmode& monitor::video_mode() const
    {
        return *_mode;
    }

    monitor::operator struct GLFWmonitor*() const
    {
        return _monitor;
    }
}
