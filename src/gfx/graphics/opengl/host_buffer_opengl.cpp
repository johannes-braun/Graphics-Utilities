#include "host_buffer_opengl.hpp"
#include <cstring>

namespace gfx::opengl
{
host_buffer_implementation::host_buffer_implementation()
{
    if(glCreateBuffers)
        glCreateBuffers(1, &_handle);
    else
    {
        glGenBuffers(1, &_handle);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, _handle);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, mygl::buffer::zero);
    }
}

host_buffer_implementation::~host_buffer_implementation()
{
    glFinish();
    if(_handle != mygl::buffer::zero)
        glDeleteBuffers(1, &_handle);
}

std::byte* host_buffer_implementation::grow(const std::byte* old_data, const size_type old_size, const size_type new_capacity)
{
    glFinish();
    if(_handle != mygl::buffer::zero)
        glDeleteBuffers(1, &_handle);

    std::byte* mapped = nullptr;
    if(glCreateBuffers)
    {
        glCreateBuffers(1, &_handle);
        glNamedBufferStorage(_handle, new_capacity, nullptr, flags);
        mapped = static_cast<std::byte*>(glMapNamedBufferRange(_handle, 0, new_capacity, map_flags));
    }
    else
    {
        glGenBuffers(1, &_handle);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, _handle);
        mapped = static_cast<std::byte*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, new_capacity, map_flags));
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, mygl::buffer::zero);
    }

    memcpy(mapped, old_data, old_size);
    glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
    return mapped;
}

std::any host_buffer_implementation::api_handle() { return _handle; }
} // namespace gfx
