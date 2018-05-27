#include "mesh.hpp"
#include <execution>
#include <gfx/camera.hpp>
#include <mygl/mygl.hpp>
#include <numeric>

namespace gfx
{
tri_mesh::instance::instance(const std::shared_ptr<const tri_mesh>& geo)
        : mesh(geo)
        , _model_buffer(1, GL_DYNAMIC_STORAGE_BIT)
{
    _last_transform.position.x = INFINITY;
}

void tri_mesh::instance::render(const camera& camera) const
{
    if(transform != _last_transform)
    {
        _model_buffer[0].matrix   = transform;
        _model_buffer[0].material = material;
        _last_transform           = transform;
        _model_buffer.synchronize();
    }

    const glm::mat4 mvp = camera.projection.matrix() * inverse(camera.transform.matrix()) * transform.matrix();

    for(int plane = 0; plane < 6; ++plane)
    {
        bool outside_plane = true;
        for(uint8_t corner = 0; corner < 8; ++corner)
        {
            const uint8_t factor_x = (corner & 0b001);
            const uint8_t factor_y = (corner & 0b010) >> 1;
            const uint8_t factor_z = (corner & 0b100) >> 2;
            glm::vec4     point    = mvp * glm::vec4(mesh->_bounds[factor_x].x, mesh->_bounds[factor_y].y, mesh->_bounds[factor_z].z, 1);
            point /= point.w;
            point.z          = point.z * 2.f - 1.f;
            const float sign = static_cast<float>(plane / 3) * 2.f - 1.f;
            outside_plane &= sign * point[plane % 3] > 1;
        }
        if(outside_plane)
            return;
    }

    _model_buffer.bind(GL_UNIFORM_BUFFER, 1);
    mesh->render();
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
        : _vertex_buffer(vertices)
        , _index_buffer(indices.empty() ? decltype(_index_buffer)() : indices)
{
    _vertex_array.attrib(0).enable(true).format(3, GL_FLOAT, false, offsetof(gfx::vertex3d, position)).bind(0);
    _vertex_array.attrib(1).enable(true).format(2, GL_FLOAT, false, offsetof(gfx::vertex3d, uv)).bind(0);
    _vertex_array.attrib(2).enable(true).format(3, GL_FLOAT, false, offsetof(gfx::vertex3d, normal)).bind(0);
    _vertex_array.vertex_buffer(0, _vertex_buffer);

    _bounds = std::reduce(std::execution::par_unseq, vertices.begin(), vertices.end(), _bounds, reduction());

    if(_index_buffer)
    {
        _vertex_array.element_buffer(*_index_buffer);
        _draw = [this] { glDrawElements(GL_TRIANGLES, _index_buffer->size(), GL_UNSIGNED_INT, nullptr); };
    }
    else
    {
        _draw = [this] { glDrawArrays(GL_TRIANGLES, 0, _vertex_buffer.size()); };
    }
}

std::unique_ptr<tri_mesh::instance> tri_mesh::instantiate() const { return std::make_unique<instance>(shared_from_this()); }

void tri_mesh::render() const
{
    _vertex_array.bind();
    _draw();
}
}
