#pragma once

#include "gfx.ecs/ecs.hpp"
#include "gfx.math/geometry.hpp"
#include "gfx.ecs.components/basic.hpp"
#include "input.hpp"
#include <optional>
#include <gfx.core/log.hpp>

namespace gfx {
inline namespace v1 {
struct camera_matrices
{
    glm::mat4 view;
    glm::mat4 projection;
    glm::vec3 position;
    int       do_cull = 1;
};
inline std::optional<camera_matrices> get_camera_info(ecs::ecs& ecs, ecs::entity_handle entity)
{
    auto* cam = ecs.get_component<projection_component>(entity);
    auto* tfm = ecs.get_component<transform_component>(entity);
    if (!cam || !tfm) return std::nullopt;
    return camera_matrices{inverse(tfm->matrix()), cam->matrix(), tfm->position, 1};
}
inline std::optional<camera_matrices> get_camera_info(const ecs::entity& entity)
{
    const auto* const cam = entity.get<projection_component>();
    const auto* const tfm = entity.get<transform_component>();
    if (!cam || !tfm) return std::nullopt;
    return camera_matrices{inverse(tfm->matrix()), cam->matrix(), tfm->position, 1};
}

struct camera_controls : ecs::component<camera_controls>
{
    key key_forward    = key::kb_w;
    key key_backward   = key::kb_s;
    key key_left       = key::kb_a;
    key key_right      = key::kb_d;
    key key_up         = key::kb_e;
    key key_down       = key::kb_q;
    key key_mod_faster = key::kb_shift;
    key key_mod_slower = key::kb_control;

    float             slow_factor    = 0.2f;
    float             fast_factor    = 12.f;
    float             rotation_speed = 1.f;
    float             movement_speed = 12.f;
    bool              inverse_y      = false;
    projection_component* last_camera    = nullptr;
    transform         target_transform;
};

class user_camera_system : public ecs::system
{
public:
    user_camera_system(input_system& input) : _input(&input)
    {
        add_component_type(projection_component::id);
        add_component_type(transform_component::id);
        add_component_type(camera_controls::id);
        add_component_type(grabbed_cursor_component::id);
    }

    void update(duration_type delta, ecs::component_base** components) const override
    {
        projection_component&     cam  = components[0]->as<projection_component>();
        transform_component&      trn  = components[1]->as<transform_component>();
        camera_controls&          ctrl = components[2]->as<camera_controls>();
        grabbed_cursor_component& gcc  = components[3]->as<grabbed_cursor_component>();

        if (&cam != ctrl.last_camera)
        {
            ctrl.last_camera      = &cam;
            ctrl.target_transform = trn;
        }

        if (!_rmb_lock && _input->button_down(button::right) && _input->get_cursor_state() == cursor_state::free)
        {
            _input->set_cursor_state(cursor_state::captured);
            _rmb_lock = true;
        }
        if (!_rmb_lock && _input->button_down(button::right) && _input->get_cursor_state() == cursor_state::captured)
        {
            _input->set_cursor_state(cursor_state::free);
            _rmb_lock = true;
        }

        if (!_input->button_down(button::right))
            _rmb_lock = false;

        const auto delta_pos = -gcc.delta;
        ctrl.target_transform.rotation =
            glm::quat(glm::vec3(0, glm::radians(delta_pos.x / (2 * glm::pi<float>())) * ctrl.rotation_speed, 0.f))
            * ctrl.target_transform.rotation;
        ctrl.target_transform.rotation *= glm::quat(
            glm::vec3(glm::radians((ctrl.inverse_y ? -1 : 1) * delta_pos.y / (2 * glm::pi<float>())) * ctrl.rotation_speed, 0.f, 0.f));
        ctrl.target_transform.position +=
            rotate(
                ctrl.target_transform.rotation,
                glm::vec3{static_cast<float>(_input->key_down(ctrl.key_right)) - static_cast<float>(_input->key_down(ctrl.key_left)),
                          static_cast<float>(_input->key_down(ctrl.inverse_y ? ctrl.key_down : ctrl.key_up))
                              - static_cast<float>(_input->key_down(ctrl.inverse_y ? ctrl.key_up : ctrl.key_down)),
                          static_cast<float>(_input->key_down(ctrl.key_backward)) - static_cast<float>(_input->key_down(ctrl.key_forward))})
            * (_input->key_down(ctrl.key_mod_faster) ? ctrl.fast_factor : (_input->key_down(ctrl.key_mod_slower) ? ctrl.slow_factor : 1.f))
            * static_cast<float>(delta.count()) * ctrl.movement_speed;

        const float alpha  = float(glm::clamp(15.0 * delta.count(), 0.0, 1.0));
        trn.position = mix(trn.position, ctrl.target_transform.position, alpha);
        trn.scale    = mix(trn.scale, ctrl.target_transform.scale, alpha);
        trn.rotation = glm::slerp(trn.rotation, ctrl.target_transform.rotation, alpha);
    }

private:
    mutable bool _rmb_lock = false;
    input_system* _input;
};
}    // namespace v1
}    // namespace gfx