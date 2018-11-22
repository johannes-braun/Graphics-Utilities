#pragma once

namespace gfx {
inline namespace v1 {
namespace ecs {
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
std::decay_t<Component>* entity::get()
{
	using type = std::decay_t<Component>;
    auto* const c = static_cast<type*>(_ecs->get_component_impl(_handle, _ecs->_components[type::id], type::id));
	assert(c);
	return c;
}
template<typename Component>
const std::decay_t<Component>* entity::get() const
{
	using type = const std::decay_t<Component>;
	auto* const c = static_cast<type*>(_ecs->get_component_impl(_handle, _ecs->_components[type::id], type::id));
	assert(c);
	return c;
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

template<typename ... Components, typename>
shared_entity ecs::create_entity_shared(const Components&... components)
{
    return shared_entity(new entity(create_entity(components...)), entity_deleter{});
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
    return static_cast<Component*>(get_component(handle, Component::id));
}
}    // namespace ecs
}    // namespace v1
}    // namespace gfx