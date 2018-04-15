#pragma once

#include "input.hpp"
#include "geometry.hpp"
#include "window.hpp"

#include <glm/glm.hpp>
#include <mygl/gl.hpp>
#include <GLFW/glfw3.h>

namespace gfx
{
    struct camera
    {
        glm::mat4 view() const;
        glm::mat4 projection() const;

        transform transform;
        float field_of_view = glm::radians(80.f);
        float clip_near = 0.1f;
        float clip_far = 1000.f;
        bool negative_y = false;
        bool inverse_z = true;

        int screen_width = 100;
        int screen_height = 100;
    };

    struct camera_controller
    {
        camera_controller(const std::shared_ptr<gfx::window>& window);
        void update(camera& camera);

        float rotation_speed = 1.f;
        float movement_speed = 12.f;

        keyboard_button btn_forward{ GLFW_KEY_W };
        keyboard_button btn_backward{ GLFW_KEY_S };
        keyboard_button btn_left{ GLFW_KEY_A };
        keyboard_button btn_right{ GLFW_KEY_D };
        keyboard_button btn_up{ GLFW_KEY_E };
        keyboard_button btn_down{ GLFW_KEY_Q };

        mouse_button grab_action{ GLFW_MOUSE_BUTTON_RIGHT };
    private:
        camera* _cam_ptr = nullptr;
        transform _target_transform;

        std::shared_ptr<gfx::window> _window;
        cursor_controls _cursor_controls;
    };
}
