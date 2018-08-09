#include "input.hpp"

namespace gfx
{
    void cursor_controls::set_mode(GLFWwindow* window, cursor_mode mode)
    {
        glfwSetInputMode(window, GLFW_CURSOR, static_cast<int>(mode));
    }

    cursor_mode cursor_controls::current_mode(GLFWwindow* window)
    {
        return static_cast<cursor_mode>(glfwGetInputMode(window, GLFW_CURSOR));
    }

    glm::dvec2 cursor_controls::update_position(GLFWwindow* window)
    {
        const bool is_initial_update = std::isnan(_current_position.x);
        const auto last_position = _current_position;
        glfwGetCursorPos(window, &_current_position.x, &_current_position.y);
        _last_position = is_initial_update ? _current_position : last_position;
        return delta();
    }

    glm::dvec2 cursor_controls::delta() const
    {
        return _current_position - _last_position;
    }
}
