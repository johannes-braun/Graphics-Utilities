#include "buffer.hpp"

namespace gl
{
    buffer::~buffer() noexcept
    {
        glDeleteBuffers(1, &_id);
    }

    buffer::operator gl_buffer_t() const noexcept
    {
        return _id;
    }

    void buffer::bind(const GLenum type, const uint32_t binding_point) const noexcept
    {
        glBindBufferRange(type, binding_point, _id, 0, _size);
    }

    void buffer::bind(const GLenum type, const uint32_t binding_point, const size_t size, const ptrdiff_t offset) const noexcept
    {
        glBindBufferRange(type, binding_point, _id, offset, size);
    }

    void buffer::clear_to_float(const float value) const noexcept
    {
        glClearNamedBufferData(_id, GL_R32F, GL_RED, GL_FLOAT, &value);
    }

    void buffer::map(const GLenum map_access) noexcept
    {
        if (_mapped_data && map_access != _map_access)
            return;
        if (_mapped_data)
            unmap();

        _map_access = map_access;
        _mapped_data = glMapNamedBufferRange(_id, 0, size(), _map_access);
    }

    void buffer::unmap() const noexcept
    {
        if (_flags & buffer_flag_bits::map_persistent)
            return;

        glUnmapNamedBuffer(_id);
    }

    void buffer::flush(const size_t size_bytes, const size_t offset_bytes) const noexcept
    {
        glFlushMappedNamedBufferRange(_id, offset_bytes, size_bytes);
    }

    size_t buffer::size() const noexcept
    {
        return _size;
    }

    uint64_t buffer::address() const noexcept
    {
        return _persistent_address;
    }

    void buffer::allocate(const size_t size, const void* data, const buffer_flags flags) const noexcept
    {
        glNamedBufferStorage(_id, size, data, static_cast<GLbitfield>(static_cast<uint32_t>(flags)));
    }

    void buffer::map_if_needed() noexcept
    {
        if (_flags & buffer_flag_bits::map_read)
        {
            if (_flags & buffer_flag_bits::map_write)
                _map_access = GL_READ_WRITE;
            else
                _map_access = GL_READ_ONLY;
        }
        else if (_flags & buffer_flag_bits::map_write)
            _map_access = GL_WRITE_ONLY;

        if (_flags & buffer_flag_bits::map_persistent)
        {
            _mapped_data = glMapNamedBuffer(_id, _map_access);
        }
    }

    void buffer::make_persistent_address() noexcept
    {
        if(!glMakeNamedBufferResidentNV)
            return;
        if (!glIsNamedBufferResidentNV(*this))
            glMakeNamedBufferResidentNV(*this, GL_READ_WRITE);
        glGetNamedBufferParameterui64vNV(*this, GL_BUFFER_GPU_ADDRESS_NV, &_persistent_address);
    }
}
