#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <optional>
#include <string_view>
#include <jpu/memory>
#include <jpu/time>
#include <filesystem>
#include "gui.hpp"

namespace io
{
    static struct glfw_pre_init
    {
        glfw_pre_init() { glfwInit(); }
        ~glfw_pre_init() { glfwTerminate(); }
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

    private:
        jpu::ref_ptr<io::gui> _gui;
        struct freer { void operator()(unsigned char* d) const { free(d); } };
        std::unique_ptr<unsigned char[], freer> _icon_storage;
        GLFWimage _icon_image;
        int _width_before_monitor{0};
        int _height_before_monitor{0};
        int _position_before_monitor_x{0};
        int _position_before_monitor_y{0};
        double _delta_time{0};
        double _last_time{0};
        GLFWwindow * _window{nullptr};
    };
}
