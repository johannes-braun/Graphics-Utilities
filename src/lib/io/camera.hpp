#pragma once

#include "button_press.hpp"
#include "cursor_controls.hpp"

#include <glm/glm.hpp>
#include <res/transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace io
{
    struct camera
    {
        glm::mat4 view() const;

        glm::mat4 projection() const
        {
            GLint last_viewport[4];
            glGetIntegerv(GL_VIEWPORT, last_viewport);
            return projection(last_viewport[2], last_viewport[3]);
        }

        glm::mat4 projection(int width, int height) const;

        res::transform transform;
        float field_of_view = glm::radians(80.f);
        float clip_near = 0.1f;
        float clip_far = 1000.f;
        bool negative_y = false;
        bool inverse_z = true;
    };

    struct default_cam_controller
    {
        void update(camera& camera, GLFWwindow* window, double delta_time);

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
        cursor_controls _cursor_controls;
    };
}
