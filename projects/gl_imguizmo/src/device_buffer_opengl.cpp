#include "device_buffer_opengl.hpp"
#include "host_buffer_opengl.hpp"

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

void device_buffer_implementation::allocate(const size_type size)
{
    if(_handle != mygl::buffer::zero)
        glDeleteBuffers(1, &_handle);
    glCreateBuffers(1, &_handle);
    glNamedBufferStorage(_handle, size, nullptr, flags);
}

void device_buffer_implementation::copy(const std::any& source, const std::any& target, const difference_type src_offset,
                                        const difference_type dst_offset, const size_type size)
{
    const auto get_handle = [](const std::any& impl) {
        if(impl.type() == typeid(detail::host_buffer_implementation*))
            return static_cast<host_buffer_implementation*>(std::any_cast<detail::host_buffer_implementation*>(impl))->handle();
        else if(impl.type() == typeid(detail::device_buffer_implementation*))
            return static_cast<device_buffer_implementation*>(std::any_cast<detail::device_buffer_implementation*>(impl))->handle();
        else
            throw std::invalid_argument("Invalid buffer implementation.");
    };

    const mygl::buffer src_handle = get_handle(source);
    const mygl::buffer dst_handle = get_handle(target);
    glCopyNamedBufferSubData(src_handle, dst_handle, src_offset, dst_offset, size);
}

void device_buffer_implementation::update(difference_type offset, size_type size, const std::byte* data)
{
    glNamedBufferSubData(_handle, offset, size, data);
}
} // namespace gfx::opengl