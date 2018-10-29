#pragma once

#include "host_buffer.hpp"
#include "implementation.hpp"
#include <any>
#include <cinttypes>
#include <api.hpp>
#include <gfx.core/flags.hpp>
#include <memory>
#include "fence.hpp"

namespace gfx {
inline namespace v1 {
template<typename T>
class device_buffer;
template<typename T>
class host_buffer;

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

namespace detail {
class device_buffer_implementation
{
public:
    static std::unique_ptr<device_buffer_implementation> make();

    using size_type       = uint64_t;
    using difference_type = int64_t;

    virtual ~device_buffer_implementation() = default;

    virtual void     update_flags(buffer_usage_flags usage)                                = 0;
    virtual void     allocate(size_type size)                                              = 0;
    virtual void     copy(const std::any& source, const std::any& target, difference_type src_offset, difference_type dst_offset,
                          size_type size, fence* f)                                                  = 0;
    virtual void     update(difference_type offset, size_type size, const std::byte* data) = 0;
    virtual std::any api_handle()                                                          = 0;
};
}    // namespace detail

enum class buf_copy_behavior
{
	resize_if_larger,
	resize_always,
	throw_if_larger,
};

template<typename From, typename To>
constexpr static bool are_copyable = false;
template<typename T>
constexpr static bool are_copyable<host_buffer<T>, device_buffer<T>> = true;
template<typename T>
constexpr static bool are_copyable<device_buffer<T>, device_buffer<T>> = true;
template<typename T>
constexpr static bool are_copyable<device_buffer<T>, host_buffer<T>> = true;

template<typename Buf>
constexpr static bool is_host_buffer = false;
template<typename T>
constexpr static bool is_host_buffer<host_buffer<T>> = true;
template<typename Buf>
constexpr static bool is_device_buffer = false;
template<typename T>
constexpr static bool is_device_buffer<device_buffer<T>> = true;

template<typename Buf>
constexpr static bool is_buffer = is_device_buffer<Buf> || is_host_buffer<Buf>;

template<typename From, typename To>
using enable_if_copiable = std::enable_if_t<are_copyable<From, To>>;

template<typename BufDst, typename BufSrc, typename = enable_if_copiable<BufSrc, BufDst>>
void buf_copy(buf_copy_behavior behavior, BufDst& dst, const BufSrc& src, size_t count, ptrdiff_t src_offset, ptrdiff_t dst_offset, fence* f = nullptr);

template<typename BufDst, typename BufSrc, typename = enable_if_copiable<BufSrc, BufDst>>
void buf_copy(BufDst& dst, const BufSrc& src, size_t count, ptrdiff_t src_offset, ptrdiff_t dst_offset, fence* f = nullptr);

template<typename BufDst, typename BufSrc, typename = enable_if_copiable<BufSrc, BufDst>>
void buf_copy(buf_copy_behavior behavior, BufDst& dst, const BufSrc& src, size_t count, fence* f = nullptr);

template<typename BufDst, typename BufSrc, typename = enable_if_copiable<BufSrc, BufDst>>
void buf_copy(BufDst& dst, const BufSrc& src, size_t count, fence* f = nullptr);

class vertex_input;
template<typename T>
class device_buffer : public impl::implements<detail::device_buffer_implementation>
{
public:
    friend class vertex_input;
    using size_type                            = int64_t;
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
    device_buffer(buffer_usage_flags usage, const host_buffer<T>& source);

    void reallocate(size_type elements);

    buffer_usage_flags usage() const noexcept;
    size_type          capacity() const noexcept;
    size_type size() const noexcept;

    void operator>>(const host_buffer<T>& buffer) const;
    void operator<<(const host_buffer<T>& buffer);
    void operator>>(const device_buffer& buffer) const;
    void operator<<(const device_buffer& buffer);

    void update(const T* data, size_type count, difference_type start = 0) const;
    void update(const std::initializer_list<T>& elements, difference_type start = 0) const;
    template<typename Container, typename = detail::enable_if_container<Container, T>>
    void update(const Container& elements, difference_type start = 0) const;

    host_buffer<T> to_host() const;

protected:
	template<typename BufDst, typename BufSrc, typename>
	friend void buf_copy(buf_copy_behavior behavior, BufDst& dst, const BufSrc& src, size_t count, ptrdiff_t src_offset, ptrdiff_t dst_offset, fence* f);

	void fill_from(const host_buffer<T>& buffer, difference_type src_offset, difference_type start, size_type count, fence* f = nullptr);
	void fill_from(const device_buffer& buffer, difference_type src_offset, difference_type start, size_type count, fence* f = nullptr);

	void copy_to(const host_buffer<T>& buffer, difference_type src_offset, difference_type dst_offset, size_type count, fence* f = nullptr) const;
	void copy_to(const device_buffer& buffer, difference_type src_offset, difference_type dst_offset, size_type count, fence* f = nullptr) const;

private:
    size_type          _size = 0;
    buffer_usage_flags _usage_flags;
};
}    // namespace v1
}    // namespace gfx

#include "device_buffer.inl"
