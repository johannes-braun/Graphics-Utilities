#include "interaction.hpp"

interaction::interaction()
{
    add_component_id(transform_component::id);
    add_component_id(collider_component::id);
}

void interaction::on_add(gfx::ecs::entity e)
{
    _entity_handles.push_back(e);
}

void interaction::on_remove(gfx::ecs::entity e)
{
    _entities_removed.push_back(e);
}

void interaction::on_add_component(gfx::ecs::entity e, gfx::ecs::id_t id)
{
    if (id == transform_component::id && e.get<collider_component>()) _entity_handles.push_back(e);
    if (id == collider_component::id && e.get<transform_component>()) _entity_handles.push_back(e);
}

void interaction::on_remove_component(gfx::ecs::entity e, gfx::ecs::id_t id)
{
    if (id == transform_component::id || id == collider_component::id) _entities_removed.push_back(e);
}
