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
}

bool glfw_input_system::key_down(key code) const
{
    return glfwGetKey(_parent, i32(code));
}

bool glfw_input_system::button_down(button code) const
{
    return glfwGetMouseButton(_parent, i32(code) - 1);
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