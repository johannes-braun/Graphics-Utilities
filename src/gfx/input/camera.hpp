#pragma once

#include "../math/geometry.hpp"
#include "input.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <mygl/mygl.hpp>

namespace gfx
{
struct camera
{
    transform  transform_mode;
    projection projection_mode{glm::radians(80.f), 100, 100, 0.1f, 1000.f, true, true};

    struct data
    {
        glm::mat4 view;
        glm::mat4 projection;
        glm::vec3 position;
        int       do_cull = 1;
    };

    data info() const noexcept;
};

struct camera_controller
{
    camera_controller();
    void update(camera& camera);

    float rotation_speed = 1.f;
    float movement_speed = 12.f;
	bool inverse_y = false;

    keyboard_button btn_forward{GLFW_KEY_W};
    keyboard_button btn_backward{GLFW_KEY_S};
    keyboard_button btn_left{GLFW_KEY_A};
    keyboard_button btn_right{GLFW_KEY_D};
    keyboard_button btn_up{GLFW_KEY_E};
    keyboard_button btn_down{GLFW_KEY_Q};

    mouse_button grab_action{GLFW_MOUSE_BUTTON_RIGHT};

    private:
    camera*         _cam_ptr = nullptr;
    transform       _target_transform;
    double          _last_time;
    cursor_controls _cursor_controls;
};
}
