#pragma once

#include "window.hpp"
#include "interface/imgui_handler.hpp"

#include <imgui/imgui.h>

namespace gfx
{
    class imgui
    {
    public:
        imgui(const std::shared_ptr<window>& w) noexcept;
        ~imgui();

        void new_frame();
        void render() const;

    private:
        void apply_theme() const;

        std::shared_ptr<window> _window;
        std::unique_ptr<imgui_handler> _handler;
        ImGuiContext* _context;

        std::array<bool, 3> _mouse_button_states;
        float _mouse_wheel_delta = 0;

        callback<GLFWkeyfun>::glfw_fun_ptr* _key_callback;
        callback<GLFWcharfun>::glfw_fun_ptr* _char_callback;
        callback<GLFWmousebuttonfun>::glfw_fun_ptr* _mousebutton_callback;
        callback<GLFWscrollfun>::glfw_fun_ptr* _scroll_callback;
    };
}
