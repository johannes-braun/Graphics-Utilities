#include "window.hpp"
#include <jpu/log>
#include "stb_image.h"

namespace io
{
    void debug_callback(GLenum source, GLenum type, GLuint id, const GLenum severity, GLsizei length, const GLchar* message, const void* user_param)
    {
        const auto format_message = [&] {
            return "source=\"" + [&]() -> std::string {
                switch (source)
                {
                case GL_DEBUG_SOURCE_API: return "API";
                case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "Window System";
                case GL_DEBUG_SOURCE_SHADER_COMPILER: return "Shader Compiler";
                case GL_DEBUG_SOURCE_THIRD_PARTY: return "Third Party";
                case GL_DEBUG_SOURCE_APPLICATION: return "Application";
                default: return "Other";
                }
            }() + "\", type=\"" + [&]() -> std::string {
                switch (type)
                {
                case GL_DEBUG_TYPE_ERROR: return "Error";
                case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated Behavior";
                case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "Undefined Behavior";
                case GL_DEBUG_TYPE_PORTABILITY: return "Portability";
                case GL_DEBUG_TYPE_PERFORMANCE: return "Performance";
                case GL_DEBUG_TYPE_MARKER: return "Marker";
                case GL_DEBUG_TYPE_PUSH_GROUP: return "Push Group";
                case GL_DEBUG_TYPE_POP_GROUP: return "Pop Group";
                default: return "Other";
                }
            }() + "\" -- " + message;
        };

        switch (severity)
        {
        case GL_DEBUG_SEVERITY_HIGH:
            log_e << format_message();
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            log_w << format_message();
            break;
        case GL_DEBUG_SEVERITY_LOW:
            log_d << format_message();
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            log_v << format_message();
            break;
        default: break;
        }
    }

    window::window(int width, int height, std::string_view title, std::optional<monitor> monitor) : window(
        width, height, title, nullptr, monitor)
    {}

    window::window(int width, int height, std::string_view title, window* share, std::optional<monitor> monitor)
    {
        _window = glfwCreateWindow(width, height, title.data(),
            monitor.has_value() ? static_cast<GLFWmonitor*>(*monitor) : nullptr,
            share ? static_cast<GLFWwindow*>(*share) : nullptr);
        glfwMakeContextCurrent(_window);
        gladLoadGL();

        glDebugMessageCallback(reinterpret_cast<GLDEBUGPROC>(&debug_callback), nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW, 0, nullptr, false);
    }

    window::~window()
    {
        glfwDestroyWindow(_window);
    }

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

    bool window::update()
    {
        glfwPollEvents();
        glfwSwapBuffers(_window);
        _delta_time = (glfwGetTime() - _last_time);
        _last_time = glfwGetTime();
        return !glfwWindowShouldClose(_window);
    }

    double window::delta_time() const
    {
        return _delta_time;
    }

    void window::close() const
    {
        glfwSetWindowShouldClose(_window, true);
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
