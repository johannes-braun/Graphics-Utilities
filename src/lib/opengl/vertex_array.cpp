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

    vertex_array::operator bool() const
    {
        return _id != 0;
    }

    vertex_array::operator unsigned() const
    {
        return _id;
    }

    void vertex_array::add_binding(const vertex_attribute_binding& binding)
    { 
        glEnableVertexArrayAttrib(_id, _attributes);
		glVertexArrayVertexBuffer(_id, _attributes, binding.buffer_id, binding.offset, binding.stride);
		glVertexArrayAttribFormat(_id, _attributes, binding.components, binding.type, binding.normalized, binding.member_offset);
		glVertexArrayAttribBinding(_id, _attributes++, 0);
    }

    void vertex_array::add_bindings(const std::vector<vertex_attribute_binding>& bindings)
    {
        for(const auto& binding : bindings)
            add_binding(binding);
    }

    void vertex_array::set_element_buffer(const uint32_t buffer) const
    {
        glVertexArrayElementBuffer(_id, buffer);
    }

    void vertex_array::bind() const
    {
		glBindVertexArray(_id);
    }
}
