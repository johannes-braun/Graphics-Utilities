#pragma once

#include <cinttypes>
#include <glad/glad.h>
#include <vector>
#include <jpu/memory>
#include <map>

namespace gl
{
    class vertex_array : public jpu::ref_count
    {
    public:
        vertex_array();
        ~vertex_array();

        void set_format(uint32_t attribute, int components, uint32_t type, bool normalized) const;
        void set_vertex_buffer(uint32_t attribute, uint32_t buffer, size_t stride, size_t offset = 0) const;
        void set_element_buffer(uint32_t buffer) const;
        void set_element_buffer(uint32_t buffer, uint32_t type, size_t stride, size_t offset = 0) const;

        void bind() const;
        operator uint32_t() const;

    private:
        uint32_t _id{ 0 };
    };
}
