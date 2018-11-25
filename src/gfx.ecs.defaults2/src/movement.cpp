#include "movement.hpp"
#include "basic.hpp"

namespace gfx {
inline namespace v1 {
movement_system::movement_system()
{
    add_component_type(transform_component::id);
    add_component_type(movement_component::id);
}

void movement_system::update(duration_type delta, ecs::component_base** components) const
{
    auto& tf = components[0]->as<transform_component>();
    auto& sp = components[1]->as<movement_component>();

    const auto sphere_radius = 1.3f;
    const auto sphere_moment = 2.f / 5.f * sphere_radius * sphere_radius;
    const auto moment        = sphere_moment;

    const auto mass            = 10.f;
    const auto weighted_moment = mass * moment;

    constexpr float movement_drag = 0;
    constexpr float rotation_drag = 0;

    const auto dt      = static_cast<float>(delta.count());
    const auto gravity = glm::vec3(0, -sp.gravity, 0);

    sp.impulse = (1.f - movement_drag) * sp.impulse + (sp.force + gravity * mass) * dt;

    tf.position += sp.impulse / mass * dt;

    sp.rotation_impulse = (1.f - rotation_drag) * sp.rotation_impulse + (sp.torque + sp.torque_acceleration) * dt;
    tf.rotation   = glm::conjugate(glm::quat(dt * sp.rotation_impulse / weighted_moment)) * tf.rotation;

    sp.torque              = glm::vec3(0.f);
    sp.torque_acceleration = glm::vec3(0.f);
}
}    // namespace v1
}    // namespace gfx