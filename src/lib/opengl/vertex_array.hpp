#pragma once

#include <cinttypes>
#include <mygl/gl.hpp>
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

        void set_format(uint32_t attribute, int components, GLenum type, bool normalized) const;
        void set_vertex_buffer(uint32_t attribute, gl_buffer_t buffer, size_t stride, size_t offset = 0) const;
        void set_element_buffer(gl_buffer_t buffer) const;

        void bind() const;
        operator gl_vertex_array_t() const;

    private:
        gl_vertex_array_t _id{ 0 };
    };
}
