#include "camera.hpp"
#include <glm/ext.hpp>

namespace io
{
    glm::mat4 camera::view() const
    {
        return glm::lookAt(transform.position,
            transform.position + transform.rotation * glm::vec3(0, 0, -1),
            transform.rotation * glm::vec3(0, 1, 0));
    }

    glm::mat4 camera::projection(const int width, const int height) const
    {
        return glm::perspectiveFov(glm::radians(70.f), static_cast<float>(width), static_cast<float>(height), 0.01f, 1000.f);
    }

    void default_cam_controller::update(camera& camera, GLFWwindow* window, const double delta_time)
    {
        grab_action.update(window);
        btn_forward.update(window);
        btn_backward.update(window);
        btn_left.update(window);
        btn_right.update(window);
        btn_up.update(window);
        btn_down.update(window);
        _cursor_controls.update_position(window);

        if (grab_action.state() == button_state::press)
            _cursor_controls.set_mode(window, _cursor_controls.current_mode(window) == cursor_mode::free ? cursor_mode::grabbed : cursor_mode::free);

        if (_cursor_controls.current_mode(window) == cursor_mode::grabbed)
        {
            const auto delta = -_cursor_controls.delta();
            camera.transform.rotation = glm::quat(glm::vec3(0, glm::radians(delta.x / 10) * rotation_speed, 0.f)) * camera.transform.rotation;
            camera.transform.rotation *= glm::quat(glm::vec3(glm::radians(delta.y / 10) * rotation_speed, 0.f, 0.f));
        }

        camera.transform.position += glm::rotate(camera.transform.rotation, glm::vec3{
            static_cast<float>(btn_right.state() == button_state::down) - static_cast<float>(btn_left.state() == button_state::down),
            static_cast<float>(btn_up.state() == button_state::down) - static_cast<float>(btn_down.state() == button_state::down),
            static_cast<float>(btn_backward.state() == button_state::down) - static_cast<float>(btn_forward.state() == button_state::down)
            }) * (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? 10.f : 1.f) * static_cast<float>(delta_time) * movement_speed;

    }
}
