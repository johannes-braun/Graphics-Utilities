#include "movement.hpp"
#include "basic.hpp"

namespace gfx {
inline namespace v1 {
movement_system::movement_system()
{
    add_component_type(transform_component::id);
    add_component_type(movement_component::id);
}

struct sphere final
{
    float radius = 1.f;
};

using shape_t = std::variant<sphere>;

struct shape_visitors_t
{
    struct moment
    {
        constexpr float operator()(const sphere& s) const noexcept
        {
            return 0.4f * s.radius * s.radius;
        }
    };
};

template<typename T, typename = decltype(shape_visitors_t::moment{}(std::declval<T>()))>
constexpr float moment_of(const T& shape)
{
    return shape_visitors_t::moment {}(shape);
}
constexpr float moment_of(const shape_t& shape)
{
    return std::visit(shape_visitors_t::moment{}, shape);
}

void movement_system::update(duration_type delta, ecs::component_base** components) const
{
    auto& tf = components[0]->as<transform_component>();
    auto& sp = components[1]->as<movement_component>();

    constexpr shape_t shape = sphere{1.3f};

    const auto moment          = moment_of(shape);
    const auto weighted_moment = sp.mass * moment;

    const auto dt      = static_cast<float>(delta.count());
    const auto gravity = glm::vec3(0, -sp.gravity, 0);

    sp.impulse = (1.f - sp.movement_drag * dt) * sp.impulse + (sp.force + gravity * sp.mass) * dt;

    tf.position += sp.impulse / sp.mass * dt;

    sp.rotation_impulse = (1.f - sp.rotation_drag * dt) * sp.rotation_impulse + (sp.torque + sp.torque_acceleration) * dt;
    tf.rotation         = glm::conjugate(glm::quat(dt * sp.rotation_impulse / weighted_moment)) * tf.rotation;

    sp.torque              = glm::vec3(0.f);
    sp.torque_acceleration = glm::vec3(0.f);
}
}    // namespace v1
}    // namespace gfx