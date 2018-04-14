#pragma once

#include "interface/callbacks.hpp"
#include "interface/window_handler.hpp"
#include "api.hpp"

#include <string_view>
#include <memory>
#include <GLFW/glfw3.h>
#include "file.hpp"

namespace gfx
{
    class window : public std::enable_shared_from_this<window>, public callbacks
    {
    public:
        window(api api, std::string_view title, uint32_t width, uint32_t height) noexcept;
        window(api api, std::string_view title, uint32_t width, uint32_t height, const std::shared_ptr<window>& share) noexcept;

        bool update();
        void close() const noexcept;

        void set_icon(const gfx::image_file& image) const noexcept;
        void set_swap_delay(double delay) noexcept;
        void set_max_framerate(double fps) noexcept;

        double swap_delay() const noexcept;
        operator GLFWwindow*() const noexcept;
        double delta_time() const noexcept;
        api context_api() const noexcept;
        
    private:
        struct glfw_delete_window { void operator()(GLFWwindow* w) const { glfwDestroyWindow(w); } };

        std::unique_ptr<GLFWwindow, glfw_delete_window> _window;
        std::shared_ptr<window> _share = nullptr;
        
        api _api;
        std::unique_ptr<window_handler> _handler;

        double _frame_begin_time = 0;
        double _frame_delta_time = 0;
        double _frame_swap_delay = 1/60.0;
    };
}
