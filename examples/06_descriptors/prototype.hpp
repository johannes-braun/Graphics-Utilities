#pragma once

#include <array>
#include <gfx.core/flags.hpp>
#include <gfx.file/file.hpp>
#include <gfx.graphics/graphics.hpp>
#include <gfx.math/datastructure/bvh.hpp>
#include <gfx.math/geometry.hpp>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace gfx {
inline namespace v1 {
class mesh_allocator;
template<typename Ii>
class prototype_instantiator;
struct mesh;

struct prototype
{
    template<typename Ii>
    friend class prototype_instantiator;
    constexpr static size_t max_submeshes = 8;
    struct submesh
    {
        mesh*          base;
        gfx::transform relative_transform;
    };

    std::string                                       name;
    std::array<std::optional<submesh>, max_submeshes> meshes{std::nullopt};

private:
    uint32_t _first_draw_command = 0;
    uint32_t _draw_command_count = 0;
};

struct mesh
{
    friend class mesh_deleter;
    friend class mesh_allocator;
    template<typename Ii>
    friend class prototype_instantiator;

private:
    u32           _index_count    = 0;
    u32           _vertex_count   = 0;
    u32           _bvh_node_count = 0;
    u32           _base_index     = 0;
    u32           _base_vertex    = 0;
    u32           _base_bvh_node  = 0;
    gfx::bounds3f _bounds;
};

using unique_prototype = std::unique_ptr<prototype, std::function<void(prototype* m)>>;
using unique_mesh      = std::unique_ptr<mesh, std::function<void(mesh* m)>>;

enum class mesh_allocator_flag
{
    use_bvh = 1 << 0,
};
using mesh_allocator_flags = gfx::flags<mesh_allocator_flag>;

class mesh_allocator
{
public:
    template<typename InstanceInfo>
    friend class prototype_instantiator;

    mesh_allocator(gfx::device& device, mesh_allocator_flags flags = {})
          : _device(&device)
          , _flags(flags)
          , _staging_vertex_buffer(*_device)
          , _staging_index_buffer(*_device)
          , _staging_bvh_buffer(*_device)
          , _bvh_generator(gfx::shape::triangle)
    {}

    mesh* allocate_mesh(const gsl::span<const gfx::vertex3d>& vertices, const gsl::span<const gfx::index32>& indices,
                        std::optional<gfx::bounds3f> bounds = std::nullopt)
    {
        mesh* m = &*_meshes.emplace_back(std::make_unique<mesh>());
        m->_base_index   = static_cast<uint32_t>(_staging_index_buffer.size());
        m->_base_vertex  = static_cast<uint32_t>(_staging_vertex_buffer.size());
        m->_index_count  = static_cast<uint32_t>(indices.size());
        m->_vertex_count = static_cast<uint32_t>(vertices.size());

        if (bounds)
            m->_bounds = *bounds;
        else
        {
            for (auto& v : vertices) m->_bounds.enclose(v.position);
        }

        _staging_index_buffer.resize(_staging_index_buffer.size() + indices.size());
        memcpy(_staging_index_buffer.data() + m->_base_index, indices.data(), indices.size() * sizeof(gfx::index32));

        if (_flags.has(mesh_allocator_flag::use_bvh))
        {
            _bvh_generator.sort(_staging_index_buffer.begin() + m->_base_index, _staging_index_buffer.end(),
                                [&](const gfx::index32 i) { return vertices[i].position; });

            m->_base_bvh_node  = static_cast<uint32_t>(_staging_bvh_buffer.size());
            m->_bvh_node_count = static_cast<uint32_t>(_bvh_generator.nodes().size());

            _staging_bvh_buffer.resize(_staging_bvh_buffer.size() + _bvh_generator.nodes().size());
            memcpy(_staging_bvh_buffer.data() + m->_base_bvh_node, _bvh_generator.nodes().data(),
                   _bvh_generator.nodes().size() * sizeof(gfx::bvh<3>::node));
        }

        _staging_vertex_buffer.resize(_staging_vertex_buffer.size() + vertices.size());
        memcpy(_staging_vertex_buffer.data() + m->_base_vertex, vertices.data(), vertices.size() * sizeof(gfx::vertex3d));

        _index_buffer = std::make_unique<gfx::buffer<gfx::index32>>(*_device, _staging_index_buffer);
        if (_flags.has(mesh_allocator_flag::use_bvh))
            _bvh_buffer = std::make_unique<gfx::buffer<gfx::bvh<3>::node>>(*_device, _staging_bvh_buffer);
        _vertex_buffer = std::make_unique<gfx::buffer<gfx::vertex3d>>(*_device, _staging_vertex_buffer);

        return m;
    }
    void free_mesh(const mesh* m)
    {
        memmove(_staging_vertex_buffer.data() + m->_base_vertex, _staging_vertex_buffer.data() + m->_base_vertex + m->_vertex_count,
                (_staging_vertex_buffer.size() - m->_base_vertex - m->_vertex_count) * sizeof(gfx::vertex3d));
        memmove(_staging_index_buffer.data() + m->_base_index, _staging_index_buffer.data() + m->_base_index + m->_index_count,
                (_staging_index_buffer.size() - m->_base_index - m->_index_count) * sizeof(gfx::index32));

        if (_flags.has(mesh_allocator_flag::use_bvh))
        {
            memmove(_staging_bvh_buffer.data() + m->_base_bvh_node, _staging_bvh_buffer.data() + m->_base_bvh_node + m->_bvh_node_count,
                    (_staging_bvh_buffer.size() - m->_base_bvh_node - m->_bvh_node_count) * sizeof(gfx::index32));
            _bvh_buffer = std::make_unique<gfx::buffer<gfx::bvh<3>::node>>(*_device, _staging_bvh_buffer);
            _staging_bvh_buffer.resize(_staging_bvh_buffer.size() - m->_bvh_node_count);
        }

        _index_buffer  = std::make_unique<gfx::buffer<gfx::index32>>(*_device, _staging_index_buffer);
        _vertex_buffer = std::make_unique<gfx::buffer<gfx::vertex3d>>(*_device, _staging_vertex_buffer);

        _staging_vertex_buffer.resize(_staging_vertex_buffer.size() - m->_vertex_count);
        _staging_index_buffer.resize(_staging_index_buffer.size() - m->_index_count);

        for (auto it = _meshes.begin(); it != _meshes.end();)
        {
            if (&**it != m)
            {
                if ((*it)->_base_index > m->_base_index) (*it)->_base_index -= m->_index_count;
                if ((*it)->_base_vertex > m->_base_vertex) (*it)->_base_vertex -= m->_vertex_count;
                if (_flags.has(mesh_allocator_flag::use_bvh) && (*it)->_base_bvh_node > m->_base_bvh_node)
                    (*it)->_base_bvh_node -= m->_bvh_node_count;
                ++it;
            }
            else
            {
                const auto d = std::distance(_meshes.begin(), it);
                std::iter_swap(it, std::prev(_meshes.end()));
                _meshes.pop_back();
                it = _meshes.begin() + d;
            }
        }
    }

    unique_mesh allocate_mesh_unique(const gsl::span<const gfx::vertex3d>& vertices, const gsl::span<const gfx::index32>& indices)
    {
        return unique_mesh(allocate_mesh(vertices, indices), [this](mesh* m) { free_mesh(m); });
    }

    const gfx::device&                    get_device() const { return *_device; }
    const gfx::buffer<gfx::vertex3d>&     vertex_buffer() const noexcept { return *_vertex_buffer; }
    const gfx::buffer<gfx::index32>&      index_buffer() const noexcept { return *_index_buffer; }
    const gfx::buffer<gfx::bvh<3>::node>& bvh_buffer() const noexcept { return *_bvh_buffer; }
    gsl::span<mesh* const>                meshes() const noexcept
    {
        auto* ms = reinterpret_cast<mesh* const*>(_meshes.data());
        return gsl::make_span(ms, _meshes.size());
    }

private:
    gfx::device*                                                _device;
    mesh_allocator_flags                                        _flags;
    std::unordered_map<std::string, std::unique_ptr<prototype>> _prototypes;
    std::vector<std::unique_ptr<mesh>>                          _meshes;

    mapped<gfx::vertex3d>     _staging_vertex_buffer;
    mapped<gfx::index32>      _staging_index_buffer;
    mapped<gfx::bvh<3>::node> _staging_bvh_buffer;

    std::unique_ptr<buffer<gfx::vertex3d>>     _vertex_buffer;
    std::unique_ptr<buffer<gfx::index32>>      _index_buffer;
    std::unique_ptr<buffer<gfx::bvh<3>::node>> _bvh_buffer;

    gfx::bvh<3> _bvh_generator;
};

enum class prototype_handle : u64 {};

template<typename InstanceInfo>
class prototype_instantiator
{
public:
    using instance_info_type = InstanceInfo;

    struct basic_instance
    {
        gfx::u32           index_count    = 0;
        gfx::u32           instance_count = 0;
        gfx::u32           base_index     = 0;
        gfx::i32           base_vertex    = 0;
        gfx::u32           base_instance  = 0;
        gfx::u32           base_bvh_node  = 0;
        gfx::u32           vertex_count   = 0;
        gfx::u32           bvh_node_count = 0;
        glm::mat4          transform;
        instance_info_type info;
    };

    prototype_instantiator(mesh_allocator& alloc) : _alloc(&alloc), _instance_descriptions(*alloc._device, 1) {}

    prototype* allocate_prototype(std::string name, std::initializer_list<mesh*> meshes)
    {
        return allocate_prototype(std::move(name), gsl::make_span(std::data(meshes), std::size(meshes)));
    }
    prototype* allocate_prototype(std::string name, const gsl::span<mesh* const>& meshes)
    {
        auto& proto = *_prototypes.emplace(name, std::make_unique<prototype>()).first->second;
        int   i     = 0;
        for (mesh* const m : meshes) proto.meshes[i++] = {m, gfx::transform{}};
        proto.name = std::move(name);
        return &proto;
    }
    void free_prototype(const prototype* proto)
    {
        assert(proto);
        _prototypes.erase(proto->name);
    }

    prototype* allocate_prototype(std::string name, const gfx::scene_file& file)
    {
        auto& proto = *_prototypes.emplace(name, std::make_unique<prototype>()).first->second;
        for (int i = 0; i < std::min(prototype::max_submeshes, file.mesh.geometries.size()); ++i)
        {
            gsl::span<const gfx::index32>  indices(file.mesh.indices.data() + file.mesh.geometries[i].base_index,
                                                  file.mesh.geometries[i].index_count);
            gsl::span<const gfx::vertex3d> vertices(file.mesh.vertices.data() + file.mesh.geometries[i].base_vertex,
                                                    file.mesh.geometries[i].vertex_count);

            proto.meshes[i] = {_alloc->allocate_mesh(vertices, indices), file.mesh.geometries[i].transformation};
        }
        proto.name = std::move(name);
        return &proto;
    }
    unique_prototype allocate_prototype_unique(std::string name, const gfx::scene_file& file)
    {
        return unique_prototype(allocate_prototype(name, file), [this](prototype* proto) { free_prototype(proto); });
    }

    unique_prototype allocate_prototype_unique(std::string name, const gsl::span<mesh*>& meshes)
    {
        return unique_prototype(allocate_prototype(name, meshes), [this](prototype* proto) { free_prototype(proto); });
    }

    prototype* prototype_by_name(const std::string& name) { return &*_prototypes.at(name); }

	prototype_handle enqueue(prototype* p, const gfx::transform& t, gsl::span<instance_info_type> properties)
    {
		static u64 current_handle = 0;
		const auto next = prototype_handle(current_handle++);
		std::pair range(_instance_descriptions.size(), 0ull);
        int i = 0;
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

    void dequeue(prototype_handle handle)
    {
		if(const auto it = _enqueued_ranges.find(handle); it != _enqueued_ranges.end())
		{
			_instance_descriptions.erase(_instance_descriptions.begin() + it->second.first, _instance_descriptions.begin() + it->second.second);
			const auto range = it->second;
			const auto diff = range.second - range.first;
            for(auto& el : _enqueued_ranges)
                if(el.second.first > range.first)
                {
					el.second.first -= diff;
					el.second.second -= diff;
                }
		}
    }

    void clear()
    {
		_enqueued_ranges.clear();
        _instance_descriptions.clear();
        _instance_descriptions.emplace_back();
    }

    const gfx::mapped<basic_instance>& instances() const noexcept { return _instance_descriptions; }

private:
    mesh_allocator*                                                _alloc;
    std::unordered_map<std::string, std::unique_ptr<prototype>>    _prototypes;
    std::unordered_map<prototype_handle, std::pair<size_t, size_t>> _enqueued_ranges;
    gfx::mapped<basic_instance>                                    _instance_descriptions;
};

}    // namespace v1
}    // namespace gfx