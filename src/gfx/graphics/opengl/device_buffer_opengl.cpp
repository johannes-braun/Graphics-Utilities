#include "device_buffer_opengl.hpp"
#include "host_buffer_opengl.hpp"
#include <stdexcept>

namespace gfx::opengl
{
device_buffer_implementation::device_buffer_implementation() { glCreateBuffers(1, &_handle); }

device_buffer_implementation::~device_buffer_implementation()
{
    if(_handle != mygl::buffer::zero)
        glDeleteBuffers(1, &_handle);
}

void device_buffer_implementation::update_flags(const buffer_usage_flags usage) { _usage = usage; }

mygl::buffer device_buffer_implementation::handle() const noexcept { return _handle; }

std::any device_buffer_implementation::api_handle() { return _handle; }

void device_buffer_implementation::allocate(const size_type size)
{
    if(_handle != mygl::buffer::zero)
        glDeleteBuffers(1, &_handle);

    if(glCreateBuffers)
    {
        glCreateBuffers(1, &_handle);
        glNamedBufferStorage(_handle, size, nullptr, flags);
    }
    else
    {
        glGenBuffers(1, &_handle);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, _handle);
        glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_DYNAMIC_COPY);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, mygl::buffer::zero);
    }
}

void device_buffer_implementation::copy(const std::any& source, const std::any& target, const difference_type src_offset,
                                        const difference_type dst_offset, const size_type size)
{
    const auto get_handle = [](const std::any& impl, bool& mapped) {
        if(impl.type() == typeid(detail::host_buffer_implementation*))
        {
            mapped = true;
            return std::any_cast<mygl::buffer>(
                    static_cast<host_buffer_implementation*>(std::any_cast<detail::host_buffer_implementation*>(impl))->api_handle());
        }
        else if(impl.type() == typeid(detail::device_buffer_implementation*))
        {
            mapped = false;
            return static_cast<device_buffer_implementation*>(std::any_cast<detail::device_buffer_implementation*>(impl))->handle();
        }
        else
            throw std::invalid_argument("Invalid buffer implementation.");
    };

    bool               src_mapped, dst_mapped;
    const mygl::buffer src_handle = get_handle(source, src_mapped);
    const mygl::buffer dst_handle = get_handle(target, dst_mapped);

    if(glCopyNamedBufferSubData)
        glCopyNamedBufferSubData(src_handle, dst_handle, src_offset, dst_offset, size);
    else
    {
        glBindBuffer(GL_COPY_READ_BUFFER, src_handle);
        glBindBuffer(GL_COPY_WRITE_BUFFER, dst_handle);
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, src_offset, dst_offset, size);
        glBindBuffer(GL_COPY_READ_BUFFER, mygl::buffer::zero);
        glBindBuffer(GL_COPY_WRITE_BUFFER, mygl::buffer::zero);
    }
    glFinish();
}

void device_buffer_implementation::update(difference_type offset, size_type size, const std::byte* data)
{
    if(glNamedBufferSubData)
        glNamedBufferSubData(_handle, offset, size, data);
    else
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, _handle);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, mygl::buffer::zero);
    }
}
} // namespace gfx::opengl
