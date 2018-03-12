#pragma once

#include "gl.hpp"

#include <cinttypes>
#include <vector>
#include <jpu/memory.hpp>
#include <map>

namespace gl
{
    class vertex_array : public jpu::ref_count
    {
    public:
        vertex_array() noexcept;
        ~vertex_array() noexcept;

        void set_format(uint32_t attribute, int components, GLenum type, bool normalized) const noexcept;
        void set_vertex_buffer(uint32_t attribute, gl_buffer_t buffer, size_t stride, size_t offset = 0) const noexcept;
        void set_element_buffer(gl_buffer_t buffer) const noexcept;

        void bind() const noexcept;
        operator gl_vertex_array_t() const noexcept;

    private:
        gl_vertex_array_t _id{ 0 };
    };
}
