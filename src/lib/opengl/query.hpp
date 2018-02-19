#pragma once

#include <glad/glad.h>
#include <jpu/memory>

namespace gl
{
    class query : public jpu::ref_count
    {
    public:
        query(GLenum type);
        ~query();

        void begin() const;
        void end() const;

        int get_int(GLenum param = GL_QUERY_RESULT) const;
        uint32_t get_uint(GLenum param = GL_QUERY_RESULT) const;
        int64_t get_int64(GLenum param = GL_QUERY_RESULT) const;
        uint64_t get_uint64(GLenum param = GL_QUERY_RESULT) const;

    private:
        GLenum _type;
        uint32_t _id;
    };
}
