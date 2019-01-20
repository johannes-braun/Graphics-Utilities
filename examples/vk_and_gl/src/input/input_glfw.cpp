#include "input_glfw.hpp"

#include <GLFW/glfw3.h>
#include <gfx.core/types.hpp>

#include <iostream>

namespace gfx {
inline namespace v1 {
glfw_input_system::glfw_input_system(GLFWwindow* parent) : _parent(parent), _cursor_state(cursor_state::free)
{
    double x, y;
    glfwGetCursorPos(_parent, &x, &y);
    _cursor_pos = {static_cast<float>(x), static_cast<float>(y)};

    glfwSetWindowUserPointer(parent, this);
    glfwSetKeyCallback(parent, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        auto&            self = *static_cast<glfw_input_system*>(glfwGetWindowUserPointer(window));
        std::unique_lock lock(self._key_mutex);
        self._key_down[gfx::key(key)] = action == GLFW_PRESS || action == GLFW_REPEAT;
    });
    glfwSetMouseButtonCallback(parent, [](GLFWwindow* window, int button, int action, int mods) {
        auto&            self = *static_cast<glfw_input_system*>(glfwGetWindowUserPointer(window));
        std::unique_lock lock(self._button_mutex);
        self._button_down[gfx::button(1<<button)] = action == GLFW_PRESS;
    });
}

bool glfw_input_system::key_down(key code) const
{
    std::unique_lock lock(_key_mutex);
    if (const auto it = _key_down.find(code); it != _key_down.end()) return it->second;
    return false;
}

bool glfw_input_system::button_down(button code) const
{
    std::unique_lock lock(_button_mutex);
    if (const auto it = _button_down.find(code); it != _button_down.end()) return it->second;
    return false;
}

void glfw_input_system::set_cursor_state(cursor_state state)
{
    _cursor_state = state;
}

cursor_state glfw_input_system::get_cursor_state() const noexcept
{
    return _cursor_state;
}

void glfw_input_system::pre_update()
{
    _last_cursor_pos = _cursor_pos;
    double x, y;
    glfwGetCursorPos(_parent, &x, &y);
    _cursor_pos = {static_cast<float>(x), static_cast<float>(y)};
    if (_cursor_state != _last_state)
    {
        _last_state           = _cursor_state;
        const auto glfw_state = [this] {
            switch (_cursor_state)
            {
            case cursor_state::free: return GLFW_CURSOR_NORMAL;
            case cursor_state::captured: return GLFW_CURSOR_DISABLED;
            default: return GLFW_CURSOR_NORMAL;
            }
        }();
        glfwSetInputMode(_parent, GLFW_CURSOR, glfw_state);
        _last_cursor_pos = _cursor_pos;
    }
}

glm::vec2 glfw_input_system::cursor_position() const
{
    return _cursor_pos;
}

glm::vec2 glfw_input_system::cursor_delta() const
{
    return _cursor_pos - _last_cursor_pos;
}
}    // namespace v1
}    // namespace gfx