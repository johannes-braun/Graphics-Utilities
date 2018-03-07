#pragma once
#include <type_traits>
#include <cinttypes>

namespace mygl
{
    enum GLenum
    {
        GL_ACCUM = 0x0100, // decimal value: 256
        GL_LOAD = 0x0101, // decimal value: 257
        GL_RETURN = 0x0102, // decimal value: 258
        GL_MULT = 0x0103, // decimal value: 259
        GL_ADD = 0x0104, // decimal value: 260

        // Buffer
        GL_COLOR = 0x1800, // decimal value: 6144
        GL_DEPTH = 0x1801, // decimal value: 6145
        GL_STENCIL = 0x1802, // decimal value: 6146
    };
    
    using GLuint = uint32_t;
    using GLintptr = int32_t;
    using GLsizeiptr = int32_t;

    using proc_addr = void*;
    proc_addr get(const char* name)
    {
        return nullptr;
    }

    void(*glTexBufferRange)(/*TextureTarget*/ GLenum target, /*InternalFormat*/ GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size);

    void load()
    {
        glTexBufferRange = reinterpret_cast<decltype(glTexBufferRange)>(get("glTexBufferRange"));
    }
}
