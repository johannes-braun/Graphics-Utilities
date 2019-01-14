#pragma once

#include <gfx.ecs/ecs.hpp>
#include <glm/glm.hpp>

namespace gfx {
inline namespace v1 {
struct movement_component : gfx::ecs::component<movement_component>
{
    float mass = 10.f;

    float     movement_drag = 1;
    glm::vec3 impulse {0, 0, 0};
    glm::vec3 force {0, 0, 0};

    float     rotation_drag = 1;
    glm::vec3 rotation_impulse {0, 0, 0};
    glm::vec3 torque {0, 0, 0};
    glm::vec3 torque_acceleration {0, 0, 0};

    float gravity = 0.f;
};

class movement_system : public gfx::ecs::system
{
public:
    movement_system();
    void update(duration_type delta, gfx::ecs::component_base** components) const override;
};
}    // namespace v1
}    // namespace gfx