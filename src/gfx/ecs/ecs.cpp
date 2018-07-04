#include "ecs.hpp"
#include <gfx/log.hpp>

namespace gfx::ecs
{
ecs::~ecs()
{
    for(auto&& cp : _components)
    {
        size_t size  = component_base::type_size(cp.first);
        auto   ffree = component_base::get_deleter(cp.first);

        for(auto i = 0ull; i < cp.second.size(); i += size)
            ffree(reinterpret_cast<component_base*>(&cp.second[i]));
    }

    for(auto&& e : _entities)
        delete e;
}
entity_handle ecs::create_entity(component_base* components, const id_t* component_ids, size_t count)
{
    indexed_entity* e   = new indexed_entity();
    entity_handle   hnd = static_cast<entity_handle>(e);

    for(auto i = 0; i < count; ++i)
    {
        if(component_base::is_valid(component_ids[i]))
        {
            gfx::elog("ecs") << "Invalid component type ID detected: " << color_fg(145)
                              << static_cast<std::underlying_type_t<id_t>>(component_ids[i]);
            delete e;
            return null_entity;
        }
        auto fcreate = component_base::get_creator(component_ids[i]);
        e->second.emplace_back(component_ids[i], fcreate(_components[component_ids[i]], hnd, &components[i]));
    }

    e->first = _entities.size();
    _entities.push_back(e);
    return hnd;
}

void ecs::delete_entity(entity_handle handle)
{
    auto& e = as_entity(handle);
    for(auto&& comp : e)
        remove_component_impl(comp.first, comp.second);
    auto dstIdx = index_of(handle);
    delete _entities[dstIdx];
    _entities[dstIdx] = _entities[_entities.size() - 1];
    _entities[dstIdx]->first = dstIdx;
    _entities.pop_back();
}
} // namespace gfx::ecs