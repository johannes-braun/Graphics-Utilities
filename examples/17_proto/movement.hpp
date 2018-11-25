#pragma once

#include <gfx.ecs/ecs.hpp>
#include <glm/glm.hpp>

struct movement_component : gfx::ecs::component<movement_component>
{
	glm::vec3 impulse{0, 0, 0};
	glm::vec3 force{0, 0, 0};
	glm::vec3 rotation_impulse{0, 0, 0};
	glm::vec3 torque{0, 0, 0};
	glm::vec3 torque_acceleration{0, 0, 0};
    float gravity = 0.f;
};

class movement_system : public gfx::ecs::system
{
public:
    movement_system();
    void update(duration_type delta, gfx::ecs::component_base** components) const override;
};
