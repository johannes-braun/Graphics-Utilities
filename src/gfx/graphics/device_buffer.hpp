#pragma once

#include "host_buffer.hpp"
#include <any>
#include <cinttypes>
#include <gfx/api.hpp>
#include <gfx/flags.hpp>
#include <memory>
#include <gfx/api.hpp>

namespace gfx
{
template <typename T> class device_buffer;
template <typename T> class host_buffer;

enum class buffer_usage : uint32_t
{
    uniform  = 1 << 0,
    storage  = 1 << 1,
    index    = 1 << 2,
    vertex   = 1 << 3,
    indirect = 1 << 4,

    all = ~0u
};
using buffer_usage_flags = gfx::flags<uint32_t, buffer_usage>;
static buffer_usage_flags operator|(buffer_usage x, buffer_usage y)
{
    return buffer_usage_flags(x) | y;
}

namespace detail
{
    class device_buffer_implementation
    {
    public:
        using size_type       = uint64_t;
        using difference_type = int64_t;

        virtual ~device_buffer_implementation() = default;

        virtual void     update_flags(buffer_usage_flags usage)                                = 0;
        virtual void     allocate(size_type size)                                              = 0;
        virtual void     copy(const std::any& source, const std::any& target, difference_type src_offset, difference_type dst_offset,
                              size_type size)                                                  = 0;
        virtual void     update(difference_type offset, size_type size, const std::byte* data) = 0;
        virtual std::any api_handle()                                                          = 0;
    };

    std::unique_ptr<device_buffer_implementation> make_device_buffer_implementation();
} // namespace detail

GFX_api_cast_template_type(gapi::opengl, device_buffer, mygl::buffer)

        class vertex_input;
template <typename T> class device_buffer
{
public:
    friend class vertex_input;
    using size_type                            = uint64_t;
    using difference_type                      = int64_t;
    using value_type                           = T;
    using pointer                              = value_type*;
    using const_pointer                        = const value_type*;
    using reference                            = value_type&;
    using const_reference                      = const value_type&;
    constexpr const static size_type type_size = sizeof(value_type);

    device_buffer(buffer_usage_flags usage = buffer_usage::all);
    device_buffer(size_type elements);
    device_buffer(buffer_usage_flags usage, size_type elements);
    device_buffer(buffer_usage_flags usage, const host_buffer<T>& source)
            : device_buffer(usage, source.size())
    {
        *this << source;
    }

    void reallocate(size_type elements);

    buffer_usage_flags usage() const noexcept;
    size_type          capacity() const noexcept;

    void operator>>(const host_buffer<T>& buffer);
    void operator<<(const host_buffer<T>& buffer);
    void operator>>(const device_buffer& buffer);
    void operator<<(const device_buffer& buffer);

    void update(const T* data, size_type count, difference_type start = 0) const;
    void update(const std::initializer_list<T>& elements, difference_type start = 0) const;
    template <typename Container, typename = detail::enable_if_container<Container, T>>
    void update(const Container& elements, difference_type start = 0) const;

    void fill_from(const host_buffer<T>& buffer, difference_type src_offset, difference_type start, size_type count);
    void fill_from(const device_buffer& buffer, difference_type src_offset, difference_type start, size_type count);

    void copy_to(const host_buffer<T>& buffer, difference_type src_offset, difference_type dst_offset, size_type count);
    void copy_to(const device_buffer& buffer, difference_type src_offset, difference_type dst_offset, size_type count);

    GFX_api_cast_op(gapi::opengl, device_buffer)

private:
    std::unique_ptr<detail::device_buffer_implementation> _implementation;
    size_type                                             _size = 0;
    buffer_usage_flags                                    _usage_flags;
};

GFX_api_cast_template_impl(gapi::opengl, device_buffer)

} // namespace gfx

#include "device_buffer.inl"