#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace gfx
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
        button(int key);
        void update(GLFWwindow* window);
        button_state state() const;
        void set_key(int key);

    private:
        int _key;
        button_state _state = button_state::up;
    };

    using mouse_button = button<&glfwGetMouseButton>;
    using keyboard_button = button<&glfwGetKey>;

    enum class cursor_mode
    {
        free = GLFW_CURSOR_NORMAL,
        grabbed = GLFW_CURSOR_DISABLED,
    };

    class cursor_controls
    {
    public:
        static void set_mode(GLFWwindow* window, cursor_mode mode);
        static cursor_mode current_mode(GLFWwindow* window);
        glm::dvec2 update_position(GLFWwindow* window);
        glm::dvec2 delta() const;

    private:
        glm::dvec2 _last_position;
        glm::dvec2 _current_position{ NAN, NAN };
    };
}

#include "button_press.inl"