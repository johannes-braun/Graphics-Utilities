#include "host_buffer_opengl.hpp"

namespace gfx::opengl
{
host_buffer_implementation::host_buffer_implementation() { glCreateBuffers(1, &_handle); }

host_buffer_implementation::~host_buffer_implementation()
{
    if(_handle != mygl::buffer::zero)
        glDeleteBuffers(1, &_handle);
}

std::byte* host_buffer_implementation::grow(const std::byte* old_data, const size_type old_size, const size_type new_capacity)
{
    if(_handle != mygl::buffer::zero)
        glDeleteBuffers(1, &_handle);
    glCreateBuffers(1, &_handle);
    glNamedBufferStorage(_handle, new_capacity, nullptr, flags);
    std::byte* mapped = static_cast<std::byte*>(glMapNamedBufferRange(_handle, 0, new_capacity, map_flags));
    memcpy(mapped, old_data, old_size);
    glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
    return mapped;
}

std::any host_buffer_implementation::api_handle() { return _handle; }
} // namespace gfx
