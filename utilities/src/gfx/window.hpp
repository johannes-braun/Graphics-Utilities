#pragma once

#include "interface/callbacks.hpp"
#include "interface/window_handler.hpp"
#include "api.hpp"
#include "geometry.hpp"

#include <string_view>
#include <memory>
#include <GLFW/glfw3.h>
#include "file.hpp"

namespace gfx
{
    using window_hints = std::map<int, int>;

    class window : public std::enable_shared_from_this<window>, public callbacks
    {
    public:
        window(const std::string& api, std::string_view title, uint32_t width, uint32_t height, const window_hints& hints = {}) noexcept;
        window(const std::string& api, std::string_view title, uint32_t width, uint32_t height, const std::shared_ptr<window>& share, const window_hints& hints ={}) noexcept;

        bool update();
        void close() const noexcept;

        void set_icon(const image_file& image) const noexcept;
        void set_swap_delay(double delay) noexcept;
        void set_max_framerate(double fps) noexcept;

        double swap_delay() const noexcept;
        operator GLFWwindow*() const noexcept;
        double delta_time() const noexcept;
        const std::string& context_api() const noexcept;
        rect2f get_rect() const noexcept;
        
    private:
        struct glfw_delete_window { void operator()(GLFWwindow* w) const { glfwDestroyWindow(w); } };

        std::unique_ptr<GLFWwindow, glfw_delete_window> _window;
        std::shared_ptr<window> _share = nullptr;
        
        std::string _api;
        std::unique_ptr<window_handler> _handler;
        
        double _frame_begin_time = 0;
        double _frame_delta_time = 0;
        double _frame_swap_delay = 1/60.0;
    };
}
