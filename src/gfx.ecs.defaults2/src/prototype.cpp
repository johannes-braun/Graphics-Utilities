#include "prototype.hpp"

namespace gfx {
inline namespace v1 {
mesh_allocator::mesh_allocator(device& device, mesh_allocator_flags flags)
      : _device(&device)
      , _flags(flags)
      , _staging_vertex_buffer(*_device)
      , _staging_index_buffer(*_device)
      , _staging_bvh_buffer(*_device)
      , _bvh_generator(shape::triangle)
{}

mesh* mesh_allocator::allocate_mesh(const mesh3d& mesh, const submesh3d& submesh)
{
    const gsl::span<const vertex3d> vertices(mesh.vertices.data() + submesh.base_vertex, submesh.vertex_count);
    const gsl::span<const index32>  indices(mesh.indices.data() + submesh.base_index, submesh.index_count);
    return allocate_mesh(vertices, indices);
}

mesh* mesh_allocator::allocate_mesh(const gsl::span<const vertex3d>& vertices, const gsl::span<const index32>& indices,
                                    std::optional<bounds3f> bounds)
{
    mesh* m          = &*_meshes.emplace_back(std::make_unique<mesh>());
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
    memcpy(_staging_index_buffer.data() + m->_base_index, indices.data(), indices.size() * sizeof(index32));

    _staging_vertex_buffer.resize(_staging_vertex_buffer.size() + vertices.size());
    memcpy(_staging_vertex_buffer.data() + m->_base_vertex, vertices.data(), vertices.size() * sizeof(vertex3d));

    if (_flags.has(mesh_allocator_flag::use_bvh))
    {
        _bvh_generator.sort(_staging_index_buffer.begin() + m->_base_index, _staging_index_buffer.end(),
                            [&](const index32 i) { return _staging_vertex_buffer[i + m->_base_vertex].position; });

        m->_base_bvh_node  = static_cast<uint32_t>(_staging_bvh_buffer.size());
        m->_bvh_node_count = static_cast<uint32_t>(_bvh_generator.nodes().size());

        _staging_bvh_buffer.resize(_staging_bvh_buffer.size() + _bvh_generator.nodes().size());
        memcpy(_staging_bvh_buffer.data() + m->_base_bvh_node, _bvh_generator.nodes().data(),
               _bvh_generator.nodes().size() * sizeof(bvh<3>::node));
    }

    if (!_index_buffer)
        _index_buffer = std::make_unique<buffer<index32>>(*_device, _staging_index_buffer);
    else
        _index_buffer->update(_staging_index_buffer);
    
    if (_flags.has(mesh_allocator_flag::use_bvh))
    {
        if (!_bvh_buffer)
            _bvh_buffer = std::make_unique<buffer<bvh<3>::node>>(*_device, _staging_bvh_buffer);
        else
            _bvh_buffer->update(_staging_bvh_buffer);
    }

    if (!_vertex_buffer)
        _vertex_buffer = std::make_unique<buffer<vertex3d>>(*_device, _staging_vertex_buffer);
    else
        _vertex_buffer->update(_staging_vertex_buffer);

    return m;
}

void mesh_allocator::free_mesh(const mesh* m)
{
    memmove(_staging_vertex_buffer.data() + m->_base_vertex, _staging_vertex_buffer.data() + m->_base_vertex + m->_vertex_count,
            (_staging_vertex_buffer.size() - m->_base_vertex - m->_vertex_count) * sizeof(vertex3d));
    memmove(_staging_index_buffer.data() + m->_base_index, _staging_index_buffer.data() + m->_base_index + m->_index_count,
            (_staging_index_buffer.size() - m->_base_index - m->_index_count) * sizeof(index32));

    if (_flags.has(mesh_allocator_flag::use_bvh))
    {
        memmove(_staging_bvh_buffer.data() + m->_base_bvh_node, _staging_bvh_buffer.data() + m->_base_bvh_node + m->_bvh_node_count,
                (_staging_bvh_buffer.size() - m->_base_bvh_node - m->_bvh_node_count) * sizeof(index32));
        _bvh_buffer = std::make_unique<buffer<bvh<3>::node>>(*_device, _staging_bvh_buffer);
        _staging_bvh_buffer.resize(_staging_bvh_buffer.size() - m->_bvh_node_count);
    }

    _index_buffer  = std::make_unique<buffer<index32>>(*_device, _staging_index_buffer);
    _vertex_buffer = std::make_unique<buffer<vertex3d>>(*_device, _staging_vertex_buffer);

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

unique_mesh mesh_allocator::allocate_mesh_unique(const gsl::span<const vertex3d>& vertices, const gsl::span<const index32>& indices,
                                                 std::optional<bounds3f> bounds)
{
    return unique_mesh(allocate_mesh(vertices, indices, bounds), [this](mesh* m) { free_mesh(m); });
}

unique_mesh mesh_allocator::allocate_mesh_unique(const mesh3d& mesh, const submesh3d& submesh)
{
    return unique_mesh(allocate_mesh(mesh, submesh), [this](gfx::mesh* m) { free_mesh(m); });
}

shared_mesh mesh_allocator::allocate_mesh_shared(const mesh3d& mesh, const submesh3d& submesh)
{
    return shared_mesh(allocate_mesh(mesh, submesh), [this](gfx::mesh* m) { free_mesh(m); });
}

shared_mesh mesh_allocator::allocate_mesh_shared(const gsl::span<const vertex3d>& vertices, const gsl::span<const index32>& indices,
                                                 std::optional<bounds3f> bounds)
{
    return shared_mesh(allocate_mesh(vertices, indices, bounds), [this](mesh* m) { free_mesh(m); });
}

const device& mesh_allocator::get_device() const
{
    return *_device;
}

const buffer<vertex3d>& mesh_allocator::vertex_buffer() const noexcept
{
    return *_vertex_buffer;
}

const buffer<index32>& mesh_allocator::index_buffer() const noexcept
{
    return *_index_buffer;
}

const buffer<bvh<3>::node>& mesh_allocator::bvh_buffer() const noexcept
{
    return *_bvh_buffer;
}

gsl::span<mesh* const> mesh_allocator::meshes() const noexcept
{
    auto* ms = reinterpret_cast<mesh* const*>(_meshes.data());
    return gsl::make_span(ms, _meshes.size());
}
}    // namespace v1
}    // namespace gfx