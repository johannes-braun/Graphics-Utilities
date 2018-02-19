#pragma once

#include <cinttypes>
#include <glad/glad.h>
#include <vector>
#include <jpu/memory>

namespace gl
{
    struct vertex_attribute_binding
    {
        vertex_attribute_binding(const uint32_t binding,
            const uint32_t buffer_id,
            const ptrdiff_t offset,
            const uint32_t components,
            const uint32_t type,
            const int32_t stride,
            const uint32_t member_offset,
            const bool normalized)
            : binding(binding),
            buffer_id(buffer_id),
            offset(offset),
            components(components),
            type(type),
            stride(stride),
            member_offset(member_offset),
            normalized(normalized)
        {}

        vertex_attribute_binding& set_binding(const uint32_t value) { binding = value; return *this; }
        vertex_attribute_binding& set_buffer_id(const uint32_t value) { buffer_id = value; return *this; }
        vertex_attribute_binding& set_offset(const uint32_t value) { offset = value; return *this; }
        vertex_attribute_binding& set_components(const uint32_t value) { components = value; return *this; }
        vertex_attribute_binding& set_type(const uint32_t value) { type = value; return *this; }
        vertex_attribute_binding& set_stride(const uint32_t value) { stride = value; return *this; }
        vertex_attribute_binding& set_member_offset(const uint32_t value) { member_offset = value; return *this; }
        vertex_attribute_binding& set_normalized(const uint32_t value) { normalized = value; return *this; }

        uint32_t binding;
        uint32_t buffer_id;
        ptrdiff_t offset;
        uint32_t components;
        uint32_t type;
        int32_t stride;
        uint32_t member_offset;
        bool normalized;
    };

    class vertex_array : public jpu::ref_count
    {
    public:
        vertex_array();
        ~vertex_array();
        operator bool() const;
        operator unsigned() const;

        void add_binding(const vertex_attribute_binding& binding);
        void add_bindings(const std::vector<vertex_attribute_binding>& bindings);
        void set_element_buffer(uint32_t buffer) const;
        void bind() const;

    private:
        uint32_t _attributes{ 0 };
        uint32_t _id{ 0 };
    };
}
