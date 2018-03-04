#include "vertex_array.hpp"

namespace gl
{
    vertex_array::vertex_array()
    {
        glCreateVertexArrays(1, &_id);
    }

    vertex_array::~vertex_array()
    {
        glDeleteVertexArrays(1, &_id);
    }

    void vertex_array::set_format(const uint32_t attribute, const int components, const uint32_t type, const bool normalized) const
    {
        glVertexArrayAttribFormat(_id, attribute, components, type, normalized, 0);
        glVertexArrayAttribBinding(_id, attribute, attribute);
    }

    void vertex_array::set_vertex_buffer(const uint32_t attribute, const uint32_t buffer, const size_t stride, const size_t offset) const
    {
        glEnableVertexArrayAttrib(_id, attribute);
        glVertexArrayVertexBuffer(_id, attribute, buffer, static_cast<GLintptr>(offset), static_cast<int>(stride));
    }

    void vertex_array::set_element_buffer(const uint32_t buffer) const
    {
        glVertexArrayElementBuffer(_id, buffer);
    }

    void vertex_array::set_element_buffer(const uint32_t buffer, const uint32_t type, const size_t stride, const size_t offset) const
    {
        glVertexArrayIndexOffsetEXT(_id, buffer, type, static_cast<int>(offset), static_cast<int>(stride));
    }

    void vertex_array::bind() const
    {
        glBindVertexArray(_id);
    }

    vertex_array::operator unsigned() const
    {
        return _id;
    }
}
