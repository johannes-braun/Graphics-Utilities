#pragma once
#include <execution>
#include <memory>
#include <numeric>
#include <any>
#include <gfx/api.hpp>

namespace std
{
enum class byte : uint8_t;
}

namespace gfx
{
namespace detail
{
    template <typename C, typename TBase>
    using enable_if_container = std::void_t<decltype(std::data(std::declval<C>())), decltype(std::size(std::declval<C>())),
                                            std::enable_if_t<!std::is_same_v<std::initializer_list<TBase>, std::decay_t<C>>>>;

    class host_buffer_implementation
    {
    public:
        using size_type                       = uint64_t;
        using difference_type                 = int64_t;
        virtual ~host_buffer_implementation() = default;

        virtual std::byte* grow(const std::byte* old_data, size_type old_size, size_type new_capacity) = 0;
        virtual std::any   api_handle() = 0;
    };
    std::unique_ptr<host_buffer_implementation> make_host_buffer_implementation();
} // namespace detail

GFX_api_cast_template_type(gapi::opengl, host_buffer, mygl::buffer)

// buffer for host memory (mapped/contiguous)
template <typename T> class device_buffer;
template <typename T> class host_buffer
{
public:
    friend class device_buffer<T>;
    using iterator_category                    = std::random_access_iterator_tag;
    using size_type                            = uint64_t;
    using difference_type                      = int64_t;
    using value_type                           = T;
    using pointer                              = value_type*;
    using const_pointer                        = const value_type*;
    using reference                            = value_type&;
    using const_reference                      = const value_type&;
    using implementation                       = detail::host_buffer_implementation;
    constexpr const static size_type type_size = sizeof(value_type);

    using iterator               = pointer;
    using const_iterator         = const_pointer;
    using reverse_iterator       = pointer;
    using const_reverse_iterator = const_pointer;

    host_buffer();
    template <bool E = std::is_default_constructible_v<value_type>, typename = std::enable_if_t<E>> host_buffer(size_type elements);
    host_buffer(size_type elements, const_reference base);
    template <typename Iter> host_buffer(Iter begin, Iter end);
    host_buffer(const std::initializer_list<T>& elements);
    template <typename Container, typename = detail::enable_if_container<Container, T>> host_buffer(const Container& elements);

    reference       at(size_type index);
    const_reference at(size_type index) const;
    reference       operator[](size_type index);
    const_reference operator[](size_type index) const;

    size_type     size() const noexcept;
    bool          empty() const noexcept;
    pointer       data() noexcept;
    const_pointer data() const noexcept;

    iterator               begin() noexcept { return _data; }
    iterator               end() noexcept { return _data + _size; }
    const_iterator         begin() const noexcept { return _data; }
    const_iterator         end() const noexcept { return _data + _size; }
    const_iterator         cbegin() const noexcept { return _data; }
    const_iterator         cend() const noexcept { return _data + _size; }
    reverse_iterator       rbegin() noexcept { return _data; }
    reverse_iterator       rend() noexcept { return _data + _size; }
    const_reverse_iterator rbegin() const noexcept { return _data; }
    const_reverse_iterator rend() const noexcept { return _data + _size; }
    const_reverse_iterator crbegin() const noexcept { return _data; }
    const_reverse_iterator crend() const noexcept { return _data + _size; }

    // Utility functions
    iterator       find(const_reference object) noexcept;
    const_iterator find(const_reference object) const noexcept;
    bool           contains(const_reference object) const noexcept;

    template <bool E = std::is_default_constructible_v<value_type>, typename = std::enable_if_t<E>> void resize(size_type elements);
    void resize(size_type elements, const_reference base);

    GFX_api_cast_op(gapi::opengl, host_buffer)

private:
    static const std::byte* byte_ptr(const_pointer ptr);
    static std::byte*       byte_ptr(pointer ptr);
    static const_pointer    type_ptr(const std::byte* ptr);
    static pointer          type_ptr(std::byte* ptr);

    std::unique_ptr<implementation> _implementation = nullptr;
    size_type                       _size           = 0;
    pointer                         _data           = nullptr;
};

GFX_api_cast_template_impl(gapi::opengl, host_buffer)

} // namespace gfx

#include "host_buffer.inl"