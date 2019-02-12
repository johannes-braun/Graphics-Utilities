#pragma once

#include "input.hpp"
#include <mutex>

struct GLFWwindow;

namespace gfx {
inline namespace v1 {
class glfw_input_system : public input_system
{
public:
    glfw_input_system(GLFWwindow* parent);
    ~glfw_input_system()                        = default;
    glfw_input_system(const glfw_input_system&) = delete;
    glfw_input_system& operator=(const glfw_input_system&) = delete;
    glfw_input_system(glfw_input_system&&)                 = default;
    glfw_input_system& operator=(glfw_input_system&&) = default;

    void pre_update() override;

    bool         key_down(key code) const override;
    bool         button_down(button code) const override;
    void         set_cursor_state(cursor_state state) override;
    cursor_state get_cursor_state() const noexcept override;
    glm::vec2    cursor_position() const override;
    glm::vec2    cursor_delta() const override;

private:
    GLFWwindow*                      _parent;
    cursor_state                     _cursor_state = cursor_state::free;
    glm::vec2                        _last_cursor_pos;
    glm::vec2                        _cursor_pos;
    cursor_state                     _last_state;
    mutable std::mutex               _key_mutex;
    std::unordered_map<key, bool>    _key_down;
    mutable std::mutex               _button_mutex;
    std::unordered_map<button, bool> _button_down;
};

}    // namespace v1
}    // namespace gfx