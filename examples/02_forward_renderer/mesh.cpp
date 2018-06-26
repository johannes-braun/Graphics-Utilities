#include "mesh.hpp"

#include <gfx/log.hpp>

namespace gfx
{
mesh_holder::mesh_holder()
        : vertex_buffer(GL_DYNAMIC_STORAGE_BIT)
        , index_buffer(GL_DYNAMIC_STORAGE_BIT)
        , info_buffer(GL_DYNAMIC_STORAGE_BIT)
{
    _vertex_array.attrib(0).enable(true).format(3, GL_FLOAT, false, offsetof(gfx::vertex3d, position)).bind(0);
    _vertex_array.attrib(1).enable(true).format(2, GL_FLOAT, false, offsetof(gfx::vertex3d, uv)).bind(0);
    _vertex_array.attrib(2).enable(true).format(3, GL_FLOAT, false, offsetof(gfx::vertex3d, normal)).bind(0);
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
    range best_vertex_range  = {static_cast<uint32_t>(vertex_buffer.size()), std::numeric_limits<uint32_t>::max()};
    for(int i = 0; i < _free_vertex_ranges.size(); ++i)
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
    range best_index_range  = {index_buffer.size(), std::numeric_limits<uint32_t>::max()};
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
        index_buffer.insert(index_buffer.end(), indices->begin(), indices->end());
    else
    {
        index_buffer.assign(index_buffer.begin() + best_index_range.begin, indices->begin(), indices->end());
        _free_index_ranges[index_range_index].begin += static_cast<uint32_t>(indices->size());
        if(_free_index_ranges[index_range_index].begin >= _free_index_ranges[index_range_index].end)
            _free_index_ranges.erase(_free_index_ranges.begin() + index_range_index);
    }

    if(vertex_range_index == -1)
        vertex_buffer.insert(vertex_buffer.end(), vertices.begin(), vertices.end());
    else
    {
        vertex_buffer.assign(vertex_buffer.begin() + best_vertex_range.begin, vertices.begin(), vertices.end());
        _free_vertex_ranges[vertex_range_index].begin += static_cast<uint32_t>(vertices.size());
        if(_free_vertex_ranges[vertex_range_index].begin >= _free_vertex_ranges[vertex_range_index].end)
            _free_vertex_ranges.erase(_free_vertex_ranges.begin() + vertex_range_index);
    }

    _vertex_array.vertex_buffer(0, vertex_buffer);
    _vertex_array.element_buffer(index_buffer);

    mesh.instance_index = static_cast<uint32_t>(info_buffer.size());
    info_buffer.push_back(mesh);

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

void mesh_holder::update_mesh(const mesh_instance& mesh)
{
    info_buffer[mesh.instance_index] = mesh;
    info_buffer.synchronize();
}

void mesh_holder::free_mesh(const mesh_instance& mesh)
{
    free_instance(mesh);

    _free_index_ranges.push_back({mesh.indirect.base_index, mesh.indirect.count});
    _free_vertex_ranges.push_back({mesh.indirect.base_vertex, mesh.indirect.vertex_count});
}

mesh_instance mesh_holder::copy_instance(const mesh_instance& instance)
{
    mesh_instance new_instance  = instance;
    new_instance.instance_index = info_buffer.size();
    info_buffer.push_back(new_instance);
    return new_instance;
}

void mesh_holder::free_instance(const mesh_instance& instance)
{
    info_buffer[instance.instance_index]                = info_buffer[info_buffer.size() - 1];
    info_buffer[instance.instance_index].instance_index = instance.instance_index;
    info_buffer.erase(std::prev(info_buffer.end()));
}
void mesh_holder::cull() const
{
    static gl::compute_pipeline cull_frustum(std::make_shared<gl::shader>("02_forward_renderer/cull_frustum.comp"));
    info_buffer.bind(GL_SHADER_STORAGE_BUFFER, 10);
    cull_frustum.dispatch(static_cast<uint32_t>(info_buffer.size()));
}

void mesh_holder::render() const
{
    _vertex_array.bind();
    info_buffer.bind(GL_SHADER_STORAGE_BUFFER, 10);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, info_buffer);
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, static_cast<int>(info_buffer.size()), sizeof(mesh_instance));
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mygl::buffer(0));
}
}
