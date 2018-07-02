#pragma once

#include <mygl/mygl.hpp>

#include <algorithm>
#include <array>
#include <cassert>
#include <cinttypes>
#include <iterator>
#include <memory>
#include <set>
#include <type_traits>
#include <utility>
#include <vector>
#include <cmath>

namespace gl
{
using byte = uint8_t;

template <typename T = byte>
struct bounded_buffer_data;
template <typename T, typename Ptr, int Inc> struct bounded_buffer_iterator_base;

template <typename T = byte> class buffer
{
    template <typename It> struct iterator_has_difference
    {
    private:
        template <typename I, typename = decltype(std::declval<I>() - std::declval<I>())>
        constexpr static bool has_diff()
        {
            return true;
        }
        template <typename I, typename... Ts> constexpr static bool has_diff(Ts..., ...)
        {
            return false;
        }

    public:
        constexpr static const bool value = has_diff<It>();
    };
    template <typename It>
    constexpr static bool iterator_has_difference_v = iterator_has_difference<It>::value;

    template <typename It>
    using is_valid_iterator = std::void_t<
            std::enable_if_t<std::is_same_v<typename std::iterator_traits<It>::value_type, T>>,
            typename std::iterator_traits<It>::difference_type,
            typename std::iterator_traits<It>::pointer,
            typename std::iterator_traits<It>::reference,
            typename std::iterator_traits<It>::value_type,
            typename std::iterator_traits<It>::iterator_category>;

public:
    using value_type      = std::decay_t<std::remove_pointer_t<T>>;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;

    using iterator               = bounded_buffer_iterator_base<T, T*, 1>;
    using const_iterator         = bounded_buffer_iterator_base<T, const T*, 1>;
    using reverse_iterator       = bounded_buffer_iterator_base<T, T*, -1>;
    using const_reverse_iterator = bounded_buffer_iterator_base<T, T*, -1>;

    using sized_type                         = std::conditional_t<std::is_same_v<void, T>, byte, T>;
    constexpr const static size_t type_size  = sizeof(sized_type);
    constexpr const static GLenum keep_usage = GLenum(~0);

    buffer(GLbitfield usage = GL_ZERO) noexcept;
    explicit buffer(size_t count, GLbitfield usage = GL_ZERO);
    explicit buffer(size_t count, const T& init, GLbitfield usage = GL_ZERO);
    buffer(std::initializer_list<T> list, GLbitfield usage = GL_ZERO);
    buffer(std::vector<T> list, GLbitfield usage = GL_ZERO);
    template<typename = std::enable_if_t<!std::is_convertible_v<T, size_t>>>
    buffer(const T& item, GLbitfield usage = GL_ZERO);
    template <size_t L> buffer(std::array<T, L> list, GLbitfield usage = GL_ZERO);
    template <typename It, typename = is_valid_iterator<It>>
    explicit buffer(It begin, It end, GLbitfield usage = GL_ZERO);
    ~buffer() noexcept;

    buffer(const buffer& other) noexcept;
    buffer(buffer&& other) noexcept;
    buffer& operator=(const buffer& other) noexcept;
    buffer& operator=(buffer&& other) noexcept;

    void                           push_back(T&& value);
    void                           push_back(const T& value);
    template <typename... As> void emplace_back(As&&... value);
    iterator                       erase(iterator it);
    template <typename It, typename = is_valid_iterator<It>>
    iterator insert(iterator at, It begin, It end);
    template <typename It, typename = is_valid_iterator<It>>
    void     assign(iterator start, It begin, It end);
    void     resize(size_t size, GLbitfield usage = keep_usage);
    void     resize(size_t size, const T& init, GLbitfield usage = keep_usage);
    void     reserve(size_t size);
    void     shrink_to_fit();
    void     map(GLbitfield access);
    void     map(size_t count, GLbitfield access);
    void     map(size_t begin, size_t count, GLbitfield access);
    void     flush_element(size_t index);
    void     flush_elements(size_t begin, size_t count);
    void     flush_bytes(size_t offset, size_t size);
    void     flush();
    void     unmap() noexcept;
    void     clear() noexcept;
    void     clear(const T& value) noexcept;
    bool     is_mapped() const noexcept;
    size_t   size() const noexcept;
    size_t   capacity() const noexcept;
    void     synchronize() const noexcept;
    uint64_t handle() const noexcept;
    bool     empty() const noexcept;
    void     bind(GLenum target, uint32_t binding) const noexcept;
    void     bind(GLenum target, uint32_t binding, ptrdiff_t offset, size_t size) const noexcept;

    T&       operator[](size_t index);
    const T& operator[](size_t index) const;
    T&       at(size_t index);
    const T& at(size_t index) const;
    T*       data() noexcept;
    const T* data() const noexcept;
             operator mygl::buffer() const noexcept;

    bounded_buffer_data<T>    iterate() noexcept;
    iterator               begin() noexcept;
    iterator               end() noexcept;
    const_iterator         begin() const noexcept;
    const_iterator         end() const noexcept;
    const_iterator         cbegin() const noexcept;
    const_iterator         cend() const noexcept;
    reverse_iterator       rbegin() noexcept;
    reverse_iterator       rend() noexcept;
    const_reverse_iterator crbegin() const noexcept;
    const_reverse_iterator crend() const noexcept;
    const_reverse_iterator rbegin() const noexcept;
    const_reverse_iterator rend() const noexcept;

private:
    double compute_size_increase() const noexcept;
    void   init_handle() noexcept;

    mygl::buffer                                                    _id   = mygl::buffer::zero;
    T*                                                              _data = nullptr;
    mutable std::array<std::pair<ptrdiff_t, std::unique_ptr<T>>, 2> _cached{
            std::make_pair(-1, nullptr), std::make_pair(-1, nullptr)};
    int        _cached_index   = 0;
    size_t     _data_size      = 0;
    size_t     _data_offset    = 0;
    bool       _data_full_size = false;
    GLbitfield _data_access    = GL_ZERO;
    GLbitfield _usage          = GL_ZERO;
    size_t     _size           = 0;
    size_t     _reserved_size  = 0;
    uint64_t   _handle         = 0;
};

template <typename T> struct bounded_buffer_data
{
    bounded_buffer_data(buffer<T>* b, size_t size);

    typename buffer<T>::iterator               begin() noexcept;
    typename buffer<T>::iterator               end() noexcept;
    typename buffer<T>::const_iterator         cbegin() const noexcept;
    typename buffer<T>::const_iterator         cend() const noexcept;
    typename buffer<T>::const_iterator         begin() const noexcept;
    typename buffer<T>::const_iterator         end() const noexcept;
    typename buffer<T>::reverse_iterator       rbegin() noexcept;
    typename buffer<T>::reverse_iterator       rend() noexcept;
    typename buffer<T>::const_reverse_iterator rbegin() const noexcept;
    typename buffer<T>::const_reverse_iterator rend() const noexcept;
    typename buffer<T>::const_reverse_iterator crbegin() const noexcept;
    typename buffer<T>::const_reverse_iterator crend() const noexcept;

private:
    buffer<T>* _parent;
    size_t  _size;
};

template <typename T, typename Ptr, int Inc>
struct bounded_buffer_iterator_base
{
    friend bounded_buffer_data<T>;
    friend buffer<T>;

    using iterator_category = std::random_access_iterator_tag;
    using value_type        = std::decay_t<std::remove_pointer_t<Ptr>>;
    using difference_type   = ptrdiff_t;
    using pointer           = Ptr;
    using reference         = value_type&;

    bool                          operator==(const bounded_buffer_iterator_base& it) const noexcept;
    bool                          operator!=(const bounded_buffer_iterator_base& it) const noexcept;
    bool                          operator<(const bounded_buffer_iterator_base& it) const noexcept;
    bool                          operator>(const bounded_buffer_iterator_base& it) const noexcept;
    bool                          operator<=(const bounded_buffer_iterator_base& it) const noexcept;
    bool                          operator>=(const bounded_buffer_iterator_base& it) const noexcept;
    bounded_buffer_iterator_base& operator+=(difference_type it) noexcept;
    bounded_buffer_iterator_base& operator-=(difference_type it) noexcept;

    bounded_buffer_iterator_base&       operator--() noexcept;
    friend bounded_buffer_iterator_base operator--(bounded_buffer_iterator_base& rhs,
                                                   int                           i) noexcept;
    bounded_buffer_iterator_base&       operator++() noexcept;
    friend bounded_buffer_iterator_base operator++(bounded_buffer_iterator_base& rhs,
                                                   int                           i) noexcept;

    bounded_buffer_iterator_base operator-(ptrdiff_t offset) const noexcept;
    bounded_buffer_iterator_base operator+(ptrdiff_t offset) const noexcept;
    ptrdiff_t operator-(const bounded_buffer_iterator_base& offset) const noexcept;

    const Ptr                                    operator->() const noexcept;
    std::enable_if_t<!std::is_const_v<Ptr>, Ptr> operator->() noexcept;

    const std::decay_t<std::remove_pointer_t<Ptr>>& operator*() const noexcept;
    std::enable_if_t<!std::is_const_v<Ptr>, std::decay_t<std::remove_pointer_t<Ptr>>&>
            operator*() noexcept;

private:
    bool compatible(const bounded_buffer_iterator_base& other) const noexcept;
    bool valid() const noexcept;

    std::conditional_t<std::is_const_v<Ptr>, const buffer<T>*, buffer<T>*> _parent;
    size_t                                                           _size;
    ptrdiff_t                                                        _offset;
};
}

#include "impl/bounded_buffer_data.inl"
#include "impl/bounded_buffer_iterator.inl"
#include "impl/buffer.inl"