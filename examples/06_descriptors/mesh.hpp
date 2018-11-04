#pragma once

#include "graphics/graphics.hpp"
#include <cinttypes>
#include <gfx.file/file.hpp>
#include <gfx.math/datastructure/bvh.hpp>
#include <gfx.math/geometry.hpp>
#include <random>

class mesh_allocator;
template<typename Info>
class mesh_instantiator;

struct mesh
{
    using u32 = gfx::u32;
    friend class mesh_allocator;
    template<typename Info>
    friend class mesh_instantiator;

private:
    u32 _index_count    = 0;
    u32 _vertex_count   = 0;
    u32 _bvh_node_count = 0;
    u32 _base_index     = 0;
    u32 _base_vertex    = 0;
    u32 _base_bvh_node  = 0;
};

struct mesh_handle
{
    friend class mesh_allocator;
    template<typename Info>
    friend class mesh_instantiator;

    mesh_handle() = default;
    mesh_handle(std::nullptr_t) {}
    mesh_handle(mesh* m) : _mesh(m) {}

private:
    mesh* _mesh = nullptr;
};

struct mesh_deleter
{
    mesh_deleter(mesh_allocator& mgr) : _mgr(mgr) {}
    void operator()(mesh* m) const;

private:
    mesh_allocator& _mgr;
};

using unique_mesh = std::unique_ptr<mesh, mesh_deleter>;

class mesh_allocator
{
public:
    template<typename Info>
    friend class mesh_instantiator;

    mesh_allocator(gfx::device& device);

    gsl::span<mesh_handle> allocate_meshes(const gfx::scene_file& scene);

    mesh_handle allocate_mesh(const gsl::span<const gfx::vertex3d>& vertices, const gsl::span<const gfx::index32>& indices);
    void        free_mesh(const mesh_handle hnd);

    unique_mesh allocate_mesh_unique(const gsl::span<const gfx::vertex3d>& vertices, const gsl::span<const gfx::index32>& indices);

    const gfx::buffer<gfx::vertex3d>&     vertex_buffer() const noexcept;
    const gfx::buffer<gfx::index32>&      index_buffer() const noexcept;
    const gfx::buffer<gfx::bvh<3>::node>& bvh_buffer() const noexcept;
    gsl::span<const mesh_handle>          meshes() const noexcept;

private:
    gfx::device*                       _device;
    std::vector<std::unique_ptr<mesh>> _meshes;

    gfx::mapped<gfx::vertex3d>     _staging_vertex_buffer;
    gfx::mapped<gfx::index32>      _staging_index_buffer;
    gfx::mapped<gfx::bvh<3>::node> _staging_bvh_buffer;

    std::optional<gfx::buffer<gfx::vertex3d>>     _vertex_buffer;
    std::optional<gfx::buffer<gfx::index32>>      _index_buffer;
    std::optional<gfx::buffer<gfx::bvh<3>::node>> _bvh_buffer;

    gfx::bvh<3> _bvh_generator;
};
template<typename InstanceInfo>
class mesh_instantiator
{
public:
    using instance_info_type = InstanceInfo;

    struct basic_instance
    {
        gfx::u32 index_count    = 0;
        gfx::u32 instance_count = 0;
        gfx::u32 base_index     = 0;
        gfx::i32 base_vertex    = 0;
        gfx::u32 base_instance  = 0;
        gfx::u32 base_bvh_node  = 0;
        gfx::u32 vertex_count   = 0;
        gfx::u32 bvh_node_count = 0;

        instance_info_type info;
    };

    mesh_instantiator(mesh_allocator& alloc) : _alloc(&alloc), _instance_descriptions(*alloc._device, 1) {}

	void clear()
	{
		_instance_descriptions.clear();
		_instance_descriptions.emplace_back();
	}

    void instantiate(const mesh_handle& m, instance_info_type info)
    {
		basic_instance& new_instance = _instance_descriptions.emplace_back();
        new_instance.base_index     = m._mesh->_base_index;
        new_instance.index_count    = m._mesh->_index_count;
        new_instance.base_instance  = 0;
        new_instance.instance_count = 1;
        new_instance.base_vertex    = m._mesh->_base_vertex;
        new_instance.vertex_count   = m._mesh->_vertex_count;
        new_instance.base_bvh_node  = m._mesh->_base_bvh_node;
        new_instance.bvh_node_count = m._mesh->_bvh_node_count;
        new_instance.info           = std::move(info);
    }

    const gfx::mapped<basic_instance>& instances() const noexcept { return _instance_descriptions; }

private:
    mesh_allocator*             _alloc;
    gfx::mapped<basic_instance> _instance_descriptions;
};