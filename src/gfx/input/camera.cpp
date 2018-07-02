#include "../camera.hpp"

#include <glm/ext.hpp>
#include <gfx/context.hpp>

namespace gfx
{
camera::data camera::info() const noexcept
{
    data d;
    d.projection = projection_mode;
    d.view       = inverse(transform_mode.matrix());
    d.position   = transform_mode.position;
    return d;
}

camera_controller::camera_controller() { _last_time = glfwGetTime(); }

void camera_controller::update(camera& camera)
{
    if(&camera != _cam_ptr)
    {
        _cam_ptr          = &camera;
        _target_transform = camera.transform_mode;
    }

    auto delta = glfwGetTime() - _last_time;
    _last_time = glfwGetTime();

    const auto      _window                       = gfx::context::current()->window();
    glm::ivec2 size;
    glfwGetFramebufferSize(_window, &size.x, &size.y);
    camera.projection_mode.perspective().screen_width  = int(size.x);
    camera.projection_mode.perspective().screen_height = int(size.y);

    grab_action.update(_window);
    btn_forward.update(_window);
    btn_backward.update(_window);
    btn_left.update(_window);
    btn_right.update(_window);
    btn_up.update(_window);
    btn_down.update(_window);
    _cursor_controls.update_position(_window);

    if(grab_action.state() == button_state::press)
        _cursor_controls.set_mode(_window,
                                  _cursor_controls.current_mode(_window) == cursor_mode::free ? cursor_mode::grabbed : cursor_mode::free);

    if(_cursor_controls.current_mode(_window) == cursor_mode::grabbed)
    {
        const auto delta = -_cursor_controls.delta();
        _target_transform.rotation =
                glm::quat(glm::vec3(0, glm::radians(delta.x / (2 * glm::pi<float>())) * rotation_speed, 0.f)) * _target_transform.rotation;
        _target_transform.rotation *= glm::quat(glm::vec3(glm::radians(delta.y / (2 * glm::pi<float>())) * rotation_speed, 0.f, 0.f));
    }

    _target_transform.position += rotate(_target_transform.rotation,
                                         glm::vec3{static_cast<float>(btn_right.state() == button_state::down) -
                                                           static_cast<float>(btn_left.state() == button_state::down),
                                                   static_cast<float>(btn_up.state() == button_state::down) -
                                                           static_cast<float>(btn_down.state() == button_state::down),
                                                   static_cast<float>(btn_backward.state() == button_state::down) -
                                                           static_cast<float>(btn_forward.state() == button_state::down)}) *
                                  (glfwGetKey(_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS
                                           ? 10.f
                                           : (glfwGetKey(_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ? 0.1f : 1.f)) *
                                  static_cast<float>(delta) * movement_speed;

    const float alpha         = float(glm::clamp(20.0 * delta, 0.0, 1.0));
    camera.transform_mode.position = mix(camera.transform_mode.position, _target_transform.position, alpha);
    camera.transform_mode.scale    = mix(camera.transform_mode.scale, _target_transform.scale, alpha);
    camera.transform_mode.rotation = glm::slerp(camera.transform_mode.rotation, _target_transform.rotation, alpha);
}
}
