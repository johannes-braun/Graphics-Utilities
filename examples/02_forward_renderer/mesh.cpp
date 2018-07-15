#define GFX_EXPOSE_APIS
#include "mesh.hpp"
#include <execution>

namespace gfx
{
mesh_holder::mesh_holder()
        : vertex_buffer(buffer_usage::storage | buffer_usage::vertex)
        , index_buffer(buffer_usage::storage | buffer_usage::index)
        , info_buffer(buffer_usage::storage | buffer_usage::indirect)
{
    _vertex_input.add_attribute(0, rgb32f, offsetof(vertex3d, position));
    _vertex_input.add_attribute(0, rg32f, offsetof(vertex3d, uv));
    _vertex_input.add_attribute(0, rgb32f, offsetof(vertex3d, normal));
    _vertex_input.set_binding_info(0, sizeof(vertex3d), input_rate::vertex);
}

mesh_instance mesh_holder::create_mesh(std::vector<gfx::vertex3d> vertices, std::optional<std::vector<gfx::index32>> indices)
{
    if(!indices)
    {
        indices = std::vector<gfx::index32>(vertices.size());

#pragma omp parallel for
        for(int64_t i = 0; i < static_cast<int64_t>(indices->size()); ++i)
            indices->at(i) = static_cast<unsigned>(i);
    }

    int   vertex_range_index = -1;
    range best_vertex_range  = {static_cast<uint32_t>(vertex_buffer.capacity()), std::numeric_limits<uint32_t>::max()};
    for(int i = 0; i < static_cast<int>(_free_vertex_ranges.size()); ++i)
    {
        range&    r     = _free_vertex_ranges[i];
        const int count = int(r.end) - r.begin;
        if(count < static_cast<int>(best_vertex_range.end) - static_cast<int>(best_vertex_range.begin) &&
           count > static_cast<int>(vertices.size()))
        {
            vertex_range_index = i;
            best_vertex_range  = r;
        }
    }

    int   index_range_index = -1;
    range best_index_range  = {static_cast<uint32_t>(index_buffer.capacity()), std::numeric_limits<uint32_t>::max()};
    for(int i = 0; i < static_cast<int>(_free_index_ranges.size()); ++i)
    {
        range&    r     = _free_index_ranges[i];
        const int count = int(r.end) - r.begin;
        if(count < (best_index_range.end - best_index_range.begin) && count > indices->size())
        {
            index_range_index = i;
            best_index_range  = r;
        }
    }

    mesh_instance mesh;

    mesh.indirect.count          = static_cast<int>(indices->size());
    mesh.indirect.vertex_count   = static_cast<int>(vertices.size());
    mesh.indirect.instance_count = 1;
    mesh.indirect.base_index     = static_cast<int>(best_index_range.begin);
    mesh.indirect.base_vertex    = static_cast<int>(best_vertex_range.begin);
    mesh.indirect.base_instance  = 0;

    if(index_range_index == -1)
    {
        host_buffer<index32> nb(index_buffer.capacity() + indices->size());
        index_buffer >> nb;
        std::copy(indices->begin(), indices->end(), nb.begin() + index_buffer.capacity());
        index_buffer.reallocate(nb.size());
        index_buffer << nb;
    }
    else
    {
        host_buffer<index32> nb(index_buffer.capacity());
        index_buffer >> nb;
        std::copy(indices->begin(), indices->end(), nb.begin() + best_index_range.begin);
        index_buffer << nb;

        _free_index_ranges[index_range_index].begin += static_cast<uint32_t>(indices->size());
        if(_free_index_ranges[index_range_index].begin >= _free_index_ranges[index_range_index].end)
            _free_index_ranges.erase(_free_index_ranges.begin() + index_range_index);
    }

    if(vertex_range_index == -1)
    {
        host_buffer<vertex3d> nb(vertex_buffer.capacity() + vertices.size());
        vertex_buffer >> nb;
        std::copy(vertices.begin(), vertices.end(), nb.begin() + vertex_buffer.capacity());
        vertex_buffer.reallocate(nb.size());
        vertex_buffer << nb;
    }
    else
    {
        host_buffer<vertex3d> nb(vertex_buffer.capacity());
        vertex_buffer >> nb;
        std::copy(vertices.begin(), vertices.end(), nb.begin() + best_vertex_range.begin);
        vertex_buffer << nb;

        _free_vertex_ranges[vertex_range_index].begin += static_cast<uint32_t>(vertices.size());
        if(_free_vertex_ranges[vertex_range_index].begin >= _free_vertex_ranges[vertex_range_index].end)
            _free_vertex_ranges.erase(_free_vertex_ranges.begin() + vertex_range_index);
    }

    mesh.instance_index = static_cast<uint32_t>(info_buffer.capacity());
    gfx::host_buffer<mesh_instance> old(info_buffer.capacity() + 1);
    info_buffer >> old;
    (*std::prev(old.end()) = mesh);
    info_buffer.reallocate(info_buffer.capacity() + 1);
    info_buffer << old;

    struct reduction
    {
        bounds3f operator()(const bounds3f& b, const vertex3d& x) const { return b + x.position; }

        bounds3f operator()(const vertex3d& x, const bounds3f& b) const { return b + x.position; }

        bounds3f operator()(const vertex3d& b, const vertex3d& x) const
        {
            bounds3f bounds;
            bounds += b.position;
            return bounds + x.position;
        }

        bounds3f operator()(const bounds3f& b, const bounds3f& x) const { return b + x; }
    };

    mesh.bounds = std::reduce(std::execution::par_unseq, vertices.begin(), vertices.end(), mesh.bounds, reduction());

    return mesh;
}

void mesh_holder::update_mesh(const mesh_instance& mesh) { info_buffer.update({mesh}, mesh.instance_index); }

void mesh_holder::free_mesh(const mesh_instance& mesh)
{
    free_instance(mesh);

    _free_index_ranges.push_back({mesh.indirect.base_index, mesh.indirect.count});
    _free_vertex_ranges.push_back({mesh.indirect.base_vertex, mesh.indirect.vertex_count});
}

mesh_instance mesh_holder::copy_instance(const mesh_instance& instance)
{
    gfx::host_buffer<mesh_instance> old(info_buffer.capacity() + 1);
    info_buffer >> old;
    auto& x          = (*std::prev(old.end()) = instance);
    x.instance_index = static_cast<uint32_t>(info_buffer.capacity());
    info_buffer.reallocate(info_buffer.capacity() + 1);
    info_buffer << old;
    return x;
}

void mesh_holder::free_instance(const mesh_instance& instance)
{
    gfx::host_buffer<mesh_instance> old(info_buffer.capacity());
    info_buffer >> old;
    old[instance.instance_index] = old[old.size() - 1];
    old[instance.instance_index].instance_index = instance.instance_index;
    info_buffer.reallocate(info_buffer.capacity() - 1);
    info_buffer << old;
}
void mesh_holder::cull() const
{
    static gl::compute_pipeline cull_frustum(std::make_shared<gl::shader>("02_forward_renderer/cull_frustum.comp"));
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, info_buffer);
    cull_frustum.dispatch(static_cast<uint32_t>(info_buffer.capacity()));
}

void mesh_holder::render()
{
    _vertex_input.bind_vertex_buffer(0, vertex_buffer, 0);
    _vertex_input.bind_index_buffer(index_buffer, index_type::uint32);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, info_buffer);
    _vertex_input.draw_indexed_indirect(info_buffer, 0, static_cast<uint32_t>(info_buffer.capacity()), sizeof(mesh_instance));
}
}
