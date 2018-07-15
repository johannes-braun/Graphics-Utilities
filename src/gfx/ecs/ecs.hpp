#pragma once

#include "system.hpp"
#include <execution>
#include <unordered_map>

namespace gfx::ecs
{
using entity_info    = std::vector<std::pair<id_t, uint32_t>>;
using indexed_entity = std::pair<uint32_t, entity_info>;

namespace traits
{
template<typename T>
struct is_component : std::is_convertible<std::decay_t<T>&, component_base&>
{
};

template<typename T>
struct is_component<component<T>> : std::true_type
{
};

template<typename T>
using enable_if_component_t = std::enable_if_t<is_component<T>::value>;
}

class ecs;
class entity
{
    friend class ecs;
    friend struct entity_deleter;

public:
    entity() = default;

    template<typename... Component>
    void add(const Component&... component);
    template<typename Component, typename... Components>
    bool remove();
    template<typename Component>
    Component* get();

    operator entity_handle() const noexcept;
    operator bool() const noexcept;

private:
    entity(ecs* e, entity_handle hnd);

    ecs*          _ecs    = nullptr;
    entity_handle _handle = nullptr;
};

struct entity_deleter
{
    void operator()(entity* e) const;
};
using unique_entity = std::unique_ptr<entity, entity_deleter>;

class ecs
{
    friend class entity;

public:
    ecs() = default;
    ~ecs();

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

    void update(double delta, system_list& list);

private:
    std::unordered_map<id_t, std::vector<std::byte>> _components;
    std::vector<indexed_entity*>                     _entities;

    void            delete_component(id_t id, size_t index);
    bool            remove_component_impl(entity_handle e, id_t component_id);
    void            add_component_impl(entity_handle e, id_t component_id, const component_base* component);
    component_base* get_component_impl(entity_handle e, std::vector<std::byte>& carr, id_t component_id);
    void update_multi_system(system_base& system, double delta, const std::vector<id_t>& types, std::vector<component_base*>& components,
                             std::vector<std::vector<std::byte>*>& carrays);

    static indexed_entity* as_entity_ptr(entity_handle handle);
    static uint32_t        index_of(entity_handle handle);
    static entity_info&    as_entity(entity_handle handle);
};
}    // namespace gfx::ecs

#include "ecs.inl"