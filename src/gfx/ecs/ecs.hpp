#pragma once

#include "entity.hpp"
#include "listener.hpp"
#include "system.hpp"
#include <execution>
#include <unordered_map>

namespace gfx {
inline namespace v1 {
namespace ecs {
class ecs
{
    friend class entity;

public:
    ecs()                 = default;
    ecs(const ecs& other) = default;
    ecs(ecs&& other)      = default;
    ecs& operator=(const ecs& other) = default;
    ecs& operator=(ecs&& other) = default;

    ~ecs();

    void add_listener(listener& l);

    entity create_entity(const component_base** components, const id_t* component_ids, size_t count);
    void   delete_entity(entity handle);

    template<typename... Components, typename = std::void_t<traits::enable_if_component_t<Components>...>>
    entity create_entity(const Components&... components);

    unique_entity create_entity_unique(const component_base** components, const id_t* component_ids, size_t count);

    template<typename... Components, typename = std::void_t<traits::enable_if_component_t<Components>...>>
    unique_entity create_entity_unique(const Components&... components);

    template<typename... Component>
    void add_components(entity_handle handle, const Component&... component);

    template<typename Component, typename... Components>
    bool remove_components(entity_handle handle);

    template<typename Component>
    Component* get_component(entity_handle handle);
	
	component_base* get_component(entity_handle handle, id_t cid);

    void update(double delta, system_list& list);

private:
    std::unordered_map<id_t, std::vector<std::byte>> _components;
    std::vector<indexed_entity*>                     _entities;
    std::vector<listener*>                           _listeners;

    void            delete_component(id_t id, size_t index);
    bool            remove_component_impl(entity_handle e, id_t component_id);
    void            add_component_impl(entity_handle e, id_t component_id, const component_base* component);
    component_base* get_component_impl(entity_handle e, std::vector<std::byte>& carr, id_t component_id);
    void update_multi_system(system_base& system, double delta, const std::vector<id_t>& types, std::vector<component_base*>& components,
                             std::vector<std::vector<std::byte>*>& component_arrays);

    static indexed_entity* as_entity_ptr(entity_handle handle);
    static uint32_t        index_of(entity_handle handle);
    static entity_info&    as_entity(entity_handle handle);
};
}    // namespace ecs
}    // namespace v1
}    // namespace gfx

#include "ecs.inl"