#pragma once

#include "gl.hpp"
#include <jpu/memory.hpp>

namespace gl
{
    class query : public jpu::ref_count
    {
    public:
        query(GLenum type) noexcept;
        ~query() noexcept;

        void begin() const noexcept;
        void end() const noexcept;

        int get_int(GLenum param = GL_QUERY_RESULT) const noexcept;
        uint32_t get_uint(GLenum param = GL_QUERY_RESULT) const noexcept;
        int64_t get_int64(GLenum param = GL_QUERY_RESULT) const noexcept;
        uint64_t get_uint64(GLenum param = GL_QUERY_RESULT) const noexcept;

    private:
        GLenum _type;
        gl_query_t _id;
    };
}
