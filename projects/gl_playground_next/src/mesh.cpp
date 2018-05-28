#include "mesh.hpp"
#include <execution>
#include <gfx/camera.hpp>
#include <mygl/mygl.hpp>
#include <numeric>

namespace gfx
{
mesh_provider& get_mesh_provider()
{
    static mesh_provider provider;
    return provider;
}

mesh_provider::mesh_provider()
        : vertex_buffer(GL_DYNAMIC_STORAGE_BIT)
        , index_buffer(GL_DYNAMIC_STORAGE_BIT)
        , info_buffer(GL_DYNAMIC_STORAGE_BIT)
{
    _vertex_array.attrib(0).enable(true).format(3, GL_FLOAT, false, offsetof(gfx::vertex3d, position)).bind(0);
    _vertex_array.attrib(1).enable(true).format(2, GL_FLOAT, false, offsetof(gfx::vertex3d, uv)).bind(0);
    _vertex_array.attrib(2).enable(true).format(3, GL_FLOAT, false, offsetof(gfx::vertex3d, normal)).bind(0);
}

void mesh_provider::render_all()
{
    get_mesh_provider()._vertex_array.vertex_buffer(0, get_mesh_provider().vertex_buffer);
    get_mesh_provider()._vertex_array.element_buffer(get_mesh_provider().index_buffer);
    get_mesh_provider()._vertex_array.bind();
    get_mesh_provider().info_buffer.bind(GL_SHADER_STORAGE_BUFFER, 10);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, get_mesh_provider().info_buffer);
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, get_mesh_provider().info_buffer.size(), sizeof(instance_info));
    // glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mygl::buffer::zero);
    glFinish();
}

tri_mesh::instance::instance(const std::shared_ptr<const tri_mesh>& geo)
        : mesh(geo)
// , _model_buffer(1, GL_DYNAMIC_STORAGE_BIT)
{
    instance_info info{0};

    if(mesh->_base_index == -1)
    {
        // Arrays
        info.indirect.arrays.count          = mesh->_draw_count;
        info.indirect.arrays.base_instance  = 0;
        info.indirect.arrays.base_vertex    = mesh->_base_vertex;
        info.indirect.arrays.instance_count = 1;
    }
    else
    {
        info.indirect.elements.count          = mesh->_draw_count;
        info.indirect.elements.base_instance  = 0;
        info.indirect.elements.base_vertex    = mesh->_base_vertex;
        info.indirect.elements.instance_count = 1;
        info.indirect.elements.base_index     = mesh->_base_index;
    }

    _info_index = static_cast<int>(get_mesh_provider().info_buffer.size());
    get_mesh_provider().info_buffer.push_back(info);

    _last_transform.position.x = INFINITY;
}

void tri_mesh::instance::render(const camera& camera) const
{
    if(transform != _last_transform)
    {
        get_mesh_provider().info_buffer[_info_index].model_matrix   = transform;
        get_mesh_provider().info_buffer[_info_index].color = material.color;
        get_mesh_provider().info_buffer[_info_index].roughness = material.roughness;
        _last_transform           = transform;
        get_mesh_provider().info_buffer.synchronize();
    }

    //const glm::mat4 mvp = camera.projection.matrix() * inverse(camera.transform.matrix()) * transform.matrix();

    //std::array<glm::vec4, 8> corners;
    //for(uint8_t corner = 0; corner < 8; ++corner)
    //{
    //    const uint8_t factor_x = (corner & 0b001);
    //    const uint8_t factor_y = (corner & 0b010) >> 1;
    //    const uint8_t factor_z = (corner & 0b100) >> 2;
    //    corners[corner]        = mvp * glm::vec4(mesh->_bounds[factor_x].x, mesh->_bounds[factor_y].y, mesh->_bounds[factor_z].z, 1);
    //    corners[corner] /= corners[corner].w;
    //    corners[corner].z = corners[corner].z * 2.f - 1.f;
    //}

    //for(int plane = 0; plane < 6; ++plane)
    //{
    //    bool outside_plane = true;
    //    for(uint8_t corner = 0; corner < 8; ++corner)
    //    {
    //        const float sign = static_cast<float>(plane / 3) * 2.f - 1.f;
    //        outside_plane &= sign * corners[corner][plane % 3] > 1;
    //    }
    //    if(outside_plane)
    //        return;
    //}

    //// _model_buffer.bind(GL_UNIFORM_BUFFER, 1);
    //mesh->render();
}
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

tri_mesh::tri_mesh(const std::vector<gfx::vertex3d>& vertices, const std::vector<gfx::index32>& indices)
{
    if(!indices.empty())
        _base_index = get_mesh_provider().index_buffer.size();
    else
        _base_index = -1;

    _base_vertex = get_mesh_provider().vertex_buffer.size();

    get_mesh_provider().index_buffer.insert(get_mesh_provider().index_buffer.end(), indices.begin(), indices.end());
    get_mesh_provider().vertex_buffer.insert(get_mesh_provider().vertex_buffer.end(), vertices.begin(), vertices.end());

    /*_vertex_array.attrib(0).enable(true).format(3, GL_FLOAT, false, offsetof(gfx::vertex3d, position)).bind(0);
    _vertex_array.attrib(1).enable(true).format(2, GL_FLOAT, false, offsetof(gfx::vertex3d, uv)).bind(0);
    _vertex_array.attrib(2).enable(true).format(3, GL_FLOAT, false, offsetof(gfx::vertex3d, normal)).bind(0);
    _vertex_array.vertex_buffer(0, get_mesh_provider().vertex_buffer);*/

    _bounds = std::reduce(std::execution::par_unseq, vertices.begin(), vertices.end(), _bounds, reduction());

    if(_base_index != -1)
    {
        _draw_count = static_cast<int>(indices.size());
        _vertex_array.element_buffer(get_mesh_provider().index_buffer);
        //_draw = [this] { _vertex_array.draw(GL_TRIANGLES, _draw_count, GL_UNSIGNED_INT, _base_index * sizeof(index32), _base_vertex); };
    }
    else
    {
        _draw_count = static_cast<int>(vertices.size());
        // _draw       = [this] { _vertex_array.draw(GL_TRIANGLES, _draw_count, _base_vertex); };
    }
}

std::unique_ptr<tri_mesh::instance> tri_mesh::instantiate() const { return std::make_unique<instance>(shared_from_this()); }

void tri_mesh::render() const
{
    //_vertex_array.bind();
    // _draw();
}
}
