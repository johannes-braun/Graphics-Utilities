#include "mesh.hpp"
#include <mygl/mygl.hpp>

namespace gfx
{
tri_mesh::instance::instance(const std::shared_ptr<const tri_mesh>& geo)
        : mesh(geo), _model_buffer(1, GL_DYNAMIC_STORAGE_BIT)
{
    _last_transform.position.x = INFINITY;
}

void tri_mesh::instance::render() const
{
    if(transform != _last_transform)
    {
        _model_buffer[0] = transform;
        _last_transform  = transform;
        _model_buffer.synchronize();
    }
    _model_buffer.bind(GL_UNIFORM_BUFFER, 1);
    mesh->render();
}

tri_mesh::tri_mesh(const std::vector<gfx::vertex3d>& vertices, const std::vector<gfx::index32>& indices)
        : _vertex_buffer(vertices)
        , _index_buffer(indices.empty() ? decltype(_index_buffer)() : indices)
{
    _vertex_array.attrib(0).enable(true).format(3, GL_FLOAT, false, offsetof(gfx::vertex3d, position)).bind(0);
    _vertex_array.attrib(1).enable(true).format(2, GL_FLOAT, false, offsetof(gfx::vertex3d, uv)).bind(0);
    _vertex_array.attrib(2).enable(true).format(3, GL_FLOAT, false, offsetof(gfx::vertex3d, normal)).bind(0);
    _vertex_array.vertex_buffer(0, _vertex_buffer);

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
