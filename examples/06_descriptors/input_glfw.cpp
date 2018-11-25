#include "input_glfw.hpp"

#include <GLFW/glfw3.h>
#include <gfx.core/types.hpp>

namespace gfx {
inline namespace v1 {
glfw_input_system::glfw_input_system(GLFWwindow* parent)
    : _parent(parent)
    , _cursor_state(cursor_state::free)
{
}

bool glfw_input_system::key_down(key code) const
{
    return glfwGetKey(_parent, u32(code)) == GLFW_PRESS;
}

bool glfw_input_system::button_down(button code) const {
    return glfwGetMouseButton(_parent, u32(code)) == GLFW_PRESS;
}

void glfw_input_system::set_cursor_state(cursor_state state)
{
    const auto glfw_state = [state]
    {
           switch(state)
           {
           case cursor_state::free: return GLFW_CURSOR_NORMAL;
           case cursor_state::captured: return GLFW_CURSOR_DISABLED;
           default: return GLFW_CURSOR_NORMAL;
           }
    }();
    _cursor_state = state;
    glfwSetInputMode(_parent, GLFW_CURSOR, glfw_state);
}

cursor_state glfw_input_system::get_cursor_state() const noexcept
{
    return _cursor_state;
}
}    // namespace v1
}    // namespace gfx