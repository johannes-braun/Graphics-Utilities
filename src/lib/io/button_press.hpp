#pragma once

#include "GLFW/glfw3.h"

namespace io
{
    enum class button_state
    {
        press = 0,
        down,
        release,
        up
    };

    template<int(*GetKey)(GLFWwindow*, int)>
    class button
    {
    public:
        explicit button(int key);
        void update(GLFWwindow* window);
        button_state state() const;
        void set_key(int key);

    private:
        int _key;
        button_state _state = button_state::up;
    };

    using mouse_button = button<&glfwGetMouseButton>;
    using keyboard_button = button<&glfwGetKey>;
}

#include "impl/button_press.inl"