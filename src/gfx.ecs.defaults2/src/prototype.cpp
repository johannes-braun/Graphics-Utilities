#include "prototype.hpp"

namespace gfx {
inline namespace v1 {
mesh_allocator::mesh_allocator(proxy* p, mesh_allocator_flags flags) : _proxy(p), _flags(flags), _bvh_generator(shape::triangle) {}

shared_mesh mesh_allocator::allocate_mesh_impl(const mesh3d& mesh, const submesh3d& submesh)
{
    const gsl::span<const vertex3d> vertices(mesh.vertices.data() + submesh.base_vertex, submesh.vertex_count);
    const gsl::span<const index32>  indices(mesh.indices.data() + submesh.base_index, submesh.index_count);
    return allocate_mesh_impl(vertices, indices);
}

shared_mesh mesh_allocator::allocate_mesh_impl(const gsl::span<const vertex3d>& vertices, const gsl::span<const index32>& indices,
                                               std::optional<bounds3f> bounds)
{
    shared_mesh m(new mesh, [this](mesh* m) { free_mesh_impl(m); });
    _meshes.emplace_back(m);

    {
        auto [vertex_buffer, index_buffer, bvh_buffer] = _proxy->data();
        m->_base_vertex                                = static_cast<uint32_t>(vertex_buffer.size());
        m->_base_index                                 = static_cast<uint32_t>(index_buffer.size());
        m->_vertex_count                               = static_cast<uint32_t>(vertices.size());
        m->_index_count                                = static_cast<uint32_t>(indices.size());
    }

    if (bounds)
        m->_bounds = *bounds;
    else
    {
        for (auto& v : vertices) m->_bounds.enclose(v.position);
    }

    _proxy->resize_stages(vertices.size(), indices.size(),0);
    auto [vertex_buffer, index_buffer, bvh_buffer] = _proxy->data();

    memcpy(vertex_buffer.data() + m->_base_vertex, vertices.data(), vertices.size() * sizeof(vertex3d));
    memcpy(index_buffer.data() + m->_base_index, indices.data(), indices.size() * sizeof(index32));

    if (_flags.has(mesh_allocator_flag::use_bvh))
    {
        _bvh_generator.sort(index_buffer.begin() + m->_base_index, index_buffer.end(),
                            [&](const index32 i) { return vertex_buffer[i + m->_base_vertex].position; });

        m->_base_bvh_node  = static_cast<uint32_t>(bvh_buffer.size());
        m->_bvh_node_count = static_cast<uint32_t>(_bvh_generator.nodes().size());

        _proxy->resize_stages(0, 0, _bvh_generator.nodes().size());
        auto [_1, _2, bvh_buffer] = _proxy->data();
        memcpy(bvh_buffer.data() + m->_base_bvh_node, _bvh_generator.nodes().data(),
               _bvh_generator.nodes().size() * sizeof(bvh<3>::node));
    }

    _proxy->update_buffers(true, true, _flags.has(mesh_allocator_flag::use_bvh));
    return m;
}

void mesh_allocator::free_mesh_impl(const mesh* m)
{
    auto [vertex_buffer, index_buffer, bvh_buffer] = _proxy->data();
    memmove(vertex_buffer.data() + m->_base_vertex, vertex_buffer.data() + m->_base_vertex + m->_vertex_count,
            (vertex_buffer.size() - m->_base_vertex - m->_vertex_count) * sizeof(vertex3d));
    memmove(index_buffer.data() + m->_base_index, index_buffer.data() + m->_base_index + m->_index_count,
            (index_buffer.size() - m->_base_index - m->_index_count) * sizeof(index32));

    if (_flags.has(mesh_allocator_flag::use_bvh))
    {
        memmove(bvh_buffer.data() + m->_base_bvh_node, bvh_buffer.data() + m->_base_bvh_node + m->_bvh_node_count,
                (bvh_buffer.size() - m->_base_bvh_node - m->_bvh_node_count) * sizeof(index32));
    }

    _proxy->resize_stages(-static_cast<ptrdiff_t>(m->_vertex_count), -static_cast<ptrdiff_t>(m->_index_count),
                          -static_cast<ptrdiff_t>(m->_bvh_node_count));
    _proxy->update_buffers(true, true, _flags.has(mesh_allocator_flag::use_bvh));

    for (auto it = _meshes.begin(); it != _meshes.end();)
    {
        if (!it->expired())
        {
            const auto locked = it->lock();
            if (&*locked != m)
            {
                if (locked->_base_index > m->_base_index) locked->_base_index -= m->_index_count;
                if (locked->_base_vertex > m->_base_vertex) locked->_base_vertex -= m->_vertex_count;
                if (_flags.has(mesh_allocator_flag::use_bvh) && locked->_base_bvh_node > m->_base_bvh_node)
                    locked->_base_bvh_node -= m->_bvh_node_count;
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
}

shared_mesh mesh_allocator::allocate_mesh(const mesh3d& mesh, const submesh3d& submesh)
{
    return allocate_mesh_impl(mesh, submesh);
}

shared_mesh mesh_allocator::allocate_mesh(const gsl::span<const vertex3d>& vertices, const gsl::span<const index32>& indices,
                                          std::optional<bounds3f> bounds)
{
    return allocate_mesh_impl(vertices, indices, bounds);
}

}    // namespace v1
}    // namespace gfx