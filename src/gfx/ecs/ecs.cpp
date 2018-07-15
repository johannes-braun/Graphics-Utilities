#include "ecs.hpp"
#include <gfx/log.hpp>

namespace gfx::ecs
{
entity::operator entity_handle() const noexcept
{
    return _handle;
}

entity::operator bool() const noexcept
{
    return _ecs && _handle;
}

entity::entity(ecs* e, entity_handle hnd)
    : _ecs(e)
    , _handle(hnd)
{
}

void entity_deleter::operator()(entity* e) const
{
    e->_ecs->delete_entity(*e);
}

ecs::~ecs()
{
    for (auto&& cp : _components) {
        size_t size  = component_base::type_size(cp.first);
        auto   ffree = component_base::get_deleter(cp.first);

        for (auto i = 0ull; i < cp.second.size(); i += size) ffree(reinterpret_cast<component_base*>(&cp.second[i]));
    }

    for (auto&& e : _entities) delete e;
}
entity ecs::create_entity(const component_base** components, const id_t* component_ids, size_t count)
{
    indexed_entity* e   = new indexed_entity();
    entity_handle   hnd = static_cast<entity_handle>(e);

    for (auto i = 0; i < count; ++i) {
        if (!component_base::is_valid(component_ids[i])) {
            gfx::elog("ecs") << "Invalid component type ID detected: " << color_fg(145)
                             << static_cast<std::underlying_type_t<id_t>>(component_ids[i]);
            delete e;
            return entity{this, null_entity};
        }
        add_component_impl(hnd, component_ids[i], components[i]);
    }

    e->first = static_cast<uint32_t>(_entities.size());
    _entities.push_back(e);
    return entity{this, hnd};
}

void ecs::delete_entity(entity handle)
{
    auto& e = as_entity(handle._handle);
    for (auto&& comp : e) delete_component(comp.first, comp.second);
    auto dst_idx = index_of(handle._handle);
    delete _entities[dst_idx];
    _entities[dst_idx]        = _entities[_entities.size() - 1];
    _entities[dst_idx]->first = dst_idx;
    _entities.pop_back();
}

unique_entity ecs::create_entity_unique(const component_base** components, const id_t* component_ids, size_t count)
{
    return unique_entity(new entity(create_entity(components, component_ids, count)));
}

void ecs::update(double delta, system_list& list)
{
    std::vector<component_base*>         multi_components;
    std::vector<std::vector<std::byte>*> component_arrays;
    for (int64_t                         i               = 0; i < static_cast<int64_t>(list.size()); ++i) {
        const auto&                      component_types = list[static_cast<uint32_t>(i)].types();
        if (component_types.size() == 1) {
            const auto size = component_base::type_size(component_types[0]);
            auto&      arr  = _components[component_types[0]];
            for (auto  ci   = 0ull; ci < arr.size(); ci += size) {
                auto*  c    = reinterpret_cast<component_base*>(&arr[ci]);
                list[static_cast<uint32_t>(i)].update(delta, &c);
            }
        } else {
            update_multi_system(list[static_cast<uint32_t>(i)], delta, component_types, multi_components, component_arrays);
        }
    }
}

void ecs::delete_component(id_t id, size_t index)
{
    auto& component_arr = _components[id];
    auto  ffree         = component_base::get_deleter(id);
    auto  size          = component_base::type_size(id);
    auto  source_index  = component_arr.size() - size;

    component_base* dst = reinterpret_cast<component_base*>(&component_arr[index]);
    component_base* src = reinterpret_cast<component_base*>(&component_arr[source_index]);
    ffree(dst);

    if (index == source_index) {
        component_arr.resize(source_index);
        return;
    }

    memcpy(dst, src, size);

    auto& components = as_entity(src->entity);
    if (const auto it = std::find_if(std::execution::par, components.begin(), components.end(),
                                     [&](const auto& p) { return p.first == id && p.second == source_index; });
        it != components.end())
    {
        it->second = static_cast<uint32_t>(index);
    }

    component_arr.resize(source_index);
}

bool ecs::remove_component_impl(entity_handle e, id_t component_id)
{
    auto& components = as_entity(e);
    if (const auto it =
            std::find_if(std::execution::par, components.begin(), components.end(), [&](const auto& p) { return p.first == component_id; });
        it != components.end())
    {
        auto src_index = components.size() - 1;
        auto dst_index = std::distance(components.begin(), it);
        delete_component(it->first, it->second);
        components[dst_index] = components[src_index];
        components.pop_back();
        return true;
    }
    return false;
}

void ecs::add_component_impl(entity_handle e, id_t component_id, const component_base* component)
{
    auto ent     = as_entity_ptr(e);
    auto fcreate = component_base::get_creator(component_id);
    ent->second.emplace_back(component_id, static_cast<uint32_t>(fcreate(_components[component_id], e, component)));
}

component_base* ecs::get_component_impl(entity_handle e, std::vector<std::byte>& carr, id_t component_id)
{
    auto& components = as_entity(e);
    if (const auto it =
            std::find_if(std::execution::par, components.begin(), components.end(), [&](const auto& p) { return p.first == component_id; });
        it != components.end())
    {
        return reinterpret_cast<component_base*>(&carr[it->second]);
    }
    return nullptr;
}

void ecs::update_multi_system(system_base& system, double delta, const std::vector<id_t>& types, std::vector<component_base*>& components,
                              std::vector<std::vector<std::byte>*>& carrays)
{
    const auto& cflags = system.flags();

    components.resize(std::max(types.size(), components.size()));
    carrays.resize(std::max(carrays.size(), components.size()));
    for (auto i = 0ull; i < types.size(); ++i) carrays[i] = &_components[types[i]];

    const auto min_index = [&]() -> ptrdiff_t {
        const auto d = std::distance(
            types.begin(), std::min_element(std::execution::par, types.begin(), types.end(), [&](const id_t& a, const id_t& b) {
                const auto sa    = component_base::type_size(a);
                const auto sb    = component_base::type_size(b);
                const auto a_opt = (cflags[std::distance(types.data(), &a)] & component_flag::optional) == component_flag::optional;
                const auto b_opt = (cflags[std::distance(types.data(), &b)] & component_flag::optional) == component_flag::optional;

                if (a_opt) return false;
                if (b_opt) return true;
                return (_components.at(a).size() / sa < _components.at(b).size() / sb);
            }));
        if (d == types.size()) return 0ll;
        return d;
    }();

    const auto size = component_base::type_size(types[min_index]);
    for (auto ci = 0ull; ci < carrays[min_index]->size(); ci += size) {
        components[min_index] = reinterpret_cast<component_base*>(&(*carrays[min_index])[ci]);
        const auto& ecomp     = components[min_index]->entity;

        if ([&]() -> bool {
                for (auto j = 0ull; j < types.size(); ++j) {
                    if (j == min_index) continue;
                    if (!(components[j] = get_component_impl(ecomp, *carrays[j], types[j]))
                        && (cflags[j] & component_flag::optional) != component_flag::optional)
                        return false;
                }
                return true;
            }())
            system.update(delta, components.data());
    }
}

indexed_entity* ecs::as_entity_ptr(entity_handle handle)
{
    return static_cast<indexed_entity*>(handle);
}

uint32_t ecs::index_of(entity_handle handle)
{
    return as_entity_ptr(handle)->first;
}

entity_info& ecs::as_entity(entity_handle handle)
{
    return as_entity_ptr(handle)->second;
}
}    // namespace gfx::ecs