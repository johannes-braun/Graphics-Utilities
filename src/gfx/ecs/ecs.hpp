#pragma once

#include "system.hpp"
#include <unordered_map>

namespace gfx::ecs
{
    using entity = std::vector<std::pair<id_t, uint32_t>>;
    using indexed_entity = std::pair<uint32_t, entity>;
class ecs
{
public:
    ecs() = default;
    ~ecs();

    entity_handle create_entity(component_base* components, const id_t* component_ids, size_t count);
    void delete_entity(entity_handle handle);
    
    template<typename Component>
    void add_component(entity_handle handle, Component* component);
    
    template<typename Component>
    void remove_component(entity_handle handle);

    template<typename Component>
    void get_component(entity_handle handle);

    void add_system(system_base* system)
    {
        _systems.push_back(system);
    }
    void remove_system(system_base* system);
    void update(double delta);

private:
    std::vector<system_base*> _systems;
    std::unordered_map<id_t, std::vector<std::byte>> _components;
    std::vector<indexed_entity*> _entities;

    void remove_component_impl(id_t id, size_t index);

    static indexed_entity* as_entity_ptr(entity_handle handle)
    {
        return static_cast<indexed_entity*>(handle);
    }

    static uint32_t index_of(entity_handle handle)
    {
        return as_entity_ptr(handle)->first;
    }

    static entity& as_entity(entity_handle handle)
    {
        return as_entity_ptr(handle)->second;
    }
};
} // namespace gfx::ecs