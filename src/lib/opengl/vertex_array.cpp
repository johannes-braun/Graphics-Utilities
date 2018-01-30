#include "vertex_array.hpp"

namespace gl
{
    void vertex_array::add_binding(const vertex_attribute_binding& binding)
    {
        glEnableVertexArrayAttrib(_id, _attributes);
		glVertexArrayVertexBuffer(_id, _attributes, binding.buffer_id, binding.offset, binding.stride);
		glVertexArrayAttribFormat(_id, _attributes, binding.components, binding.type, binding.normalized, binding.member_offset);
		glVertexArrayAttribBinding(_id, _attributes++, binding.binding);
    }

    void vertex_array::add_bindings(const std::vector<vertex_attribute_binding>& bindings)
    {
        for(const auto& binding : bindings)
            add_binding(binding);
    }

    void vertex_array::set_element_buffer(uint32_t buffer)
    {
        glVertexArrayElementBuffer(_id, buffer);
    }

    void vertex_array::bind()
    {
		glBindVertexArray(_id);
    }
}
