#include "../vertex_array.hpp"

namespace gl
{
    vertex_array::vertex_array() noexcept
    {
        glCreateVertexArrays(1, &_id);
    }

    vertex_array::~vertex_array() noexcept
    {
        glDeleteVertexArrays(1, &_id);
    }

    void vertex_array::set_format(const uint32_t attribute, const int components, const GLenum type, const bool normalized) const noexcept
    {
        glVertexArrayAttribFormat(_id, attribute, components, type, normalized, 0);
        glVertexArrayAttribBinding(_id, attribute, attribute);
    }

    void vertex_array::set_vertex_buffer(const uint32_t attribute, const gl_buffer_t buffer, const size_t stride, const size_t offset) const noexcept
    {
        glEnableVertexArrayAttrib(_id, attribute);
        glVertexArrayVertexBuffer(_id, attribute, buffer, static_cast<int>(offset), static_cast<int>(stride));
    }

    void vertex_array::set_element_buffer(const gl_buffer_t buffer) const noexcept
    {
        glVertexArrayElementBuffer(_id, buffer);
    }
    
    void vertex_array::bind() const noexcept
    {
        glBindVertexArray(_id);
    }

    vertex_array::operator gl_vertex_array_t() const noexcept
    {
        return _id;
    }
}
