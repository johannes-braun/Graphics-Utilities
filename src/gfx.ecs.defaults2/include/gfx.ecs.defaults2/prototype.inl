#pragma once

namespace gfx {
inline namespace v1 {
template<typename InstanceInfo>
prototype_instantiator<InstanceInfo>::prototype_instantiator(mesh_allocator& alloc)
      : _alloc(&alloc), _instance_descriptions(*alloc._device, 1)
{}

template<typename InstanceInfo>
prototype* prototype_instantiator<InstanceInfo>::allocate_prototype(std::string name, std::initializer_list<mesh*> meshes)
{
    return allocate_prototype(std::move(name), gsl::make_span(std::data(meshes), std::size(meshes)));
}

template<typename InstanceInfo>
prototype* prototype_instantiator<InstanceInfo>::allocate_prototype(std::string name, const gsl::span<mesh* const>& meshes)
{
    auto& proto = *_prototypes.emplace(name, std::make_unique<prototype>()).first->second;
    int   i     = 0;
    for (mesh* const m : meshes) proto.meshes[i++] = {m, transform{}};
    proto.name = std::move(name);
    return &proto;
}

template<typename InstanceInfo>
void prototype_instantiator<InstanceInfo>::free_prototype(const prototype* proto)
{
    assert(proto);
    _prototypes.erase(proto->name);
}

template<typename InstanceInfo>
prototype* prototype_instantiator<InstanceInfo>::allocate_prototype(std::string name, const mesh3d& m)
{
    auto& proto = *_prototypes.emplace(name, std::make_unique<prototype>()).first->second;
    for (int i = 0; i < std::min(prototype::max_submeshes, m.geometries.size()); ++i)
    {
        proto.meshes[i] = {_alloc->allocate_mesh(m, m.geometries[i]), m.geometries[i].transformation};
    }
    proto.name = std::move(name);
    return &proto;
}

template<typename InstanceInfo>
unique_prototype prototype_instantiator<InstanceInfo>::allocate_prototype_unique(std::string name, const mesh3d& m)
{
    return unique_prototype(allocate_prototype(name, m), [this](prototype* proto) { free_prototype(proto); });
}

template<typename InstanceInfo>
unique_prototype prototype_instantiator<InstanceInfo>::allocate_prototype_unique(std::string name, const gsl::span<mesh*>& meshes)
{
    return unique_prototype(allocate_prototype(name, meshes), [this](prototype* proto) { free_prototype(proto); });
}

template<typename InstanceInfo>
shared_prototype prototype_instantiator<InstanceInfo>::allocate_prototype_shared(std::string name, const mesh3d& m)
{
    return shared_prototype(allocate_prototype(name, m), [this](prototype* proto) { free_prototype(proto); });
}

template<typename InstanceInfo>
shared_prototype prototype_instantiator<InstanceInfo>::allocate_prototype_shared(std::string name, const gsl::span<mesh*>& meshes)
{
    return shared_prototype(allocate_prototype(name, meshes), [this](prototype* proto) { free_prototype(proto); });
}

template<typename InstanceInfo>
prototype* prototype_instantiator<InstanceInfo>::prototype_by_name(const std::string& name)
{
    return &*_prototypes.at(name);
}

template<typename InstanceInfo>
prototype_handle prototype_instantiator<InstanceInfo>::enqueue(prototype* p, const transform& t, gsl::span<instance_info_type> properties)
{
    static u64 current_handle = 0;
    const auto next           = prototype_handle(current_handle++);
    std::pair  range(_instance_descriptions.size(), 0ull);
    int        i = 0;
    for (const auto& m : p->meshes)
    {
        if (!m) break;
        basic_instance& new_instance = _instance_descriptions.emplace_back();
        new_instance.base_index      = m->base->_base_index;
        new_instance.index_count     = m->base->_index_count;
        new_instance.base_instance   = 0;
        new_instance.instance_count  = 1;
        new_instance.base_vertex     = m->base->_base_vertex;
        new_instance.vertex_count    = m->base->_vertex_count;
        new_instance.base_bvh_node   = m->base->_base_bvh_node;
        new_instance.bvh_node_count  = m->base->_bvh_node_count;
        new_instance.transform       = t * m->relative_transform;
        new_instance.info            = properties[i];
        ++i;
    }
    range.second = range.first + i;
    _enqueued_ranges.emplace(next, range);
    return next;
}

template<typename InstanceInfo>
void prototype_instantiator<InstanceInfo>::dequeue(prototype_handle handle)
{
    if (const auto it = _enqueued_ranges.find(handle); it != _enqueued_ranges.end())
    {
        _instance_descriptions.erase(_instance_descriptions.begin() + it->second.first, _instance_descriptions.begin() + it->second.second);
        const auto range = it->second;
        const auto diff  = range.second - range.first;
        for (auto& el : _enqueued_ranges)
            if (el.second.first > range.first)
            {
                el.second.first -= diff;
                el.second.second -= diff;
            }
    }
}

template<typename InstanceInfo>
void prototype_instantiator<InstanceInfo>::clear()
{
    _enqueued_ranges.clear();
    _instance_descriptions.clear();
    _instance_descriptions.emplace_back();
}

template<typename InstanceInfo>
const mapped<typename prototype_instantiator<InstanceInfo>::basic_instance>& prototype_instantiator<InstanceInfo>::instances() const
    noexcept
{
    return _instance_descriptions;
}

template<typename Info>
instance_system<Info>::instance_system(device& device, mesh_allocator_flags flags) : _alloc(device, flags), _instantiator(_alloc)
{
    add_component_type<instance_type>();
    add_component_type<transform_component>();
}

template<typename Info>
void instance_system<Info>::pre_update()
{
    _instantiator.clear();
}

template<typename Info>
void instance_system<Info>::update(duration_type delta, ecs::component_base** components) const
{
    auto& inst      = components[0]->as<instance_type>();
    auto& transform = components[1]->as<transform_component>();

    std::vector<Info> infos;
    for (int i = 0; i < prototype::max_submeshes; ++i)
    {
        if (!inst.handle->meshes[i]) break;
        infos.push_back({inst.info});
    }
    _instantiator.enqueue(inst.handle, transform, infos);
}

template<typename Info>
const prototype_instantiator<Info>& instance_system<Info>::get_instantiator() const noexcept
{
    return _instantiator;
}

template<typename Info>
const mesh_allocator& instance_system<Info>::get_mesh_allocator() const noexcept
{
    return _alloc;
}

template<typename Info>
prototype_instantiator<Info>& instance_system<Info>::get_instantiator() noexcept
{
    return _instantiator;
}

template<typename Info>
mesh_allocator& instance_system<Info>::get_mesh_allocator() noexcept
{
    return _alloc;
}
}    // namespace v1
}    // namespace gfx