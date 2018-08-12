#pragma once

namespace gfx::ecs
{
template<typename... Component>
void entity::add(const Component&... component)
{
    (_ecs->add_component_impl(_handle, Component::id, &component), ...);
}

template<typename Component, typename... Components>
bool entity::remove()
{
    return (_ecs->remove_component_impl(_handle, Component::id) && ... && _ecs->remove_component_impl(_handle, Components::id));
}

template<typename Component>
Component* entity::get()
{
    return static_cast<Component*>(_ecs->get_component_impl(_handle, _ecs->_components[Component::id], Component::id));
}

template<typename... Components, typename>
entity ecs::create_entity(const Components&... components)
{
    if constexpr (sizeof...(Components) == 0)
        return create_entity(nullptr, nullptr, 0);
    else
    {
        const component_base* css[]{static_cast<const component_base*>(&components)...};
        id_t                  ids[]{components.id...};
        return create_entity(css, ids, sizeof...(Components));
    }
}

template<typename... Components, typename>
unique_entity ecs::create_entity_unique(const Components&... components)
{
    return unique_entity(new entity(create_entity(components...)));
}

template<typename... Component>
void ecs::add_components(entity_handle handle, const Component&... component)
{
    (add_component_impl(handle, Component::id, &component), ...);
}

template<typename Component, typename... Components>
bool ecs::remove_components(entity_handle handle)
{
    return (remove_component_impl(handle, Component::id) && ... && remove_component_impl(handle, Components::id));
}

template<typename Component>
Component* ecs::get_component(entity_handle handle)
{
    return static_cast<Component*>(get_component_impl(handle, _components[Component::id], Component::id));
}
}