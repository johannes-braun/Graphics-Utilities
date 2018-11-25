#pragma once

#include "input.hpp"

struct GLFWwindow;

namespace gfx
{
    inline namespace v1
    {
        class glfw_input_system : public input_system
        {
        public:
            glfw_input_system(GLFWwindow* parent);
            ~glfw_input_system() = default;
            glfw_input_system(const glfw_input_system&) = delete;
            glfw_input_system& operator=(const glfw_input_system&) = delete;
            glfw_input_system(glfw_input_system&&) = default;
            glfw_input_system& operator=(glfw_input_system&&) = default;

            bool key_down(key code) const override;
            bool button_down(button code) const override;
            void set_cursor_state(cursor_state state) override;
            cursor_state get_cursor_state() const noexcept override;
        private:
            GLFWwindow* _parent;
            cursor_state _cursor_state;
        };

    }
}