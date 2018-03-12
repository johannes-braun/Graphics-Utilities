#pragma once

#include <mygl/gl.hpp>
#include <type_traits>
#include <cinttypes>
#include <algorithm>
#include <utility>
#include <iterator>
#include <memory>
#include <jpu/log.hpp>
#include <set>

namespace gl::v2
{
    using byte = uint8_t;

    template<typename It>
    struct iterator_has_difference {
    private:
        template<typename I, typename = decltype(std::declval<I>() - std::declval<I>())> constexpr static bool has_diff() { return true; }
        template<typename I, typename... Ts> constexpr static bool has_diff(Ts..., ...) { return false; }
    public:
        constexpr static const bool value = has_diff<It>();
    };
    template<typename It> constexpr static bool iterator_has_difference_v = iterator_has_difference<It>::value;

    template<typename T = byte>
    class buffer
    {
    public:
        using sized_type = std::conditional_t<std::is_same_v<void, T>, byte, T>;
        constexpr const static size_t type_size = sizeof(sized_type);
        constexpr const static GLenum keep_usage = GLenum(~0);

        buffer(GLbitfield usage = GL_ZERO) noexcept;
        explicit buffer(size_t count, GLbitfield usage = GL_ZERO);
        explicit buffer(size_t count, const T& init, GLbitfield usage = GL_ZERO);
        explicit buffer(std::initializer_list<T> list, GLbitfield usage = GL_ZERO);
        template<typename It,
            typename = std::enable_if_t<std::is_same_v<typename std::iterator_traits<It>::value_type, T>>,
            typename = typename std::iterator_traits<It>::difference_type,
            typename = typename std::iterator_traits<It>::pointer,
            typename = typename std::iterator_traits<It>::reference,
            typename = typename std::iterator_traits<It>::value_type,
            typename = typename std::iterator_traits<It>::iterator_category
        >
        explicit buffer(It begin, It end, GLbitfield usage = GL_ZERO);
        ~buffer() noexcept;

        void push_back(T&& value);
        void push_back(const T& value);
        template<typename... As> void emplace_back(As&&... value);
        void resize(size_t size, GLbitfield usage = keep_usage);
        void resize(size_t size, const T& init, GLbitfield usage = keep_usage);
        void reserve(size_t size);
        void shrink_to_fit();
        void map(GLbitfield access);
        void map(size_t count, GLbitfield access);
        void map(size_t begin, size_t count, GLbitfield access);
        void flush_element(size_t index);
        void flush_elements(size_t begin, size_t count);
        void flush_bytes(size_t offset, size_t size);
        void unmap() noexcept;
        bool is_mapped() const noexcept;
        size_t size() const noexcept;
        size_t capacity() const noexcept;
        void synchronize() const noexcept;
        uint64_t handle() const noexcept;

        T& operator[](size_t index);
        const T& operator[](size_t index) const;
        T& at(size_t index);
        const T& at(size_t index) const;
        T* data() noexcept;
        const T* data() const noexcept;
        operator gl_buffer_t() const noexcept;
        
        struct bounded_buffer_data;
        template<typename Ptr, int Inc>
        struct bounded_buffer_iterator_base
        {
            friend bounded_buffer_data;
            friend buffer;

            using iterator_category = std::random_access_iterator_tag;
            using value_type = std::decay_t<std::remove_pointer_t<Ptr>>;
            using difference_type = ptrdiff_t;
            using pointer = Ptr;
            using reference = value_type & ;

            bool operator==(const bounded_buffer_iterator_base& it) const;
            bool operator!=(const bounded_buffer_iterator_base& it) const;

            bool operator<(const bounded_buffer_iterator_base& it) const;
            bool operator>(const bounded_buffer_iterator_base& it) const;

            bounded_buffer_iterator_base& operator--();
            friend bounded_buffer_iterator_base operator--(bounded_buffer_iterator_base& rhs, int i);
            bounded_buffer_iterator_base& operator++();
            friend bounded_buffer_iterator_base operator++(bounded_buffer_iterator_base& rhs, int i);

            bounded_buffer_iterator_base operator-(ptrdiff_t offset) const;
            bounded_buffer_iterator_base operator+(ptrdiff_t offset) const;

            ptrdiff_t operator-(const bounded_buffer_iterator_base& offset) const;

            const Ptr operator->() const;
            std::enable_if_t<!std::is_const_v<Ptr>, Ptr> operator->();

            const std::decay_t<std::remove_pointer_t<Ptr>>& operator*() const;
            std::enable_if_t<!std::is_const_v<Ptr>, std::decay_t<std::remove_pointer_t<Ptr>>&> operator*();

        private:
            bool compatible(const bounded_buffer_iterator_base& other) const;
            bool valid() const;

            const buffer * _parent;
            size_t _size;
            ptrdiff_t _offset;
        };

        using iterator = bounded_buffer_iterator_base<T*, 1>;
        using const_iterator = bounded_buffer_iterator_base<const T*, 1>;
        using reverse_iterator = bounded_buffer_iterator_base<T*, -1>;
        using const_reverse_iterator = bounded_buffer_iterator_base<T*, -1>;

        struct bounded_buffer_data
        {
            iterator begin() noexcept {
                iterator i; i._parent = _parent; i._size = _size; i._offset = 0i64; return i;
            }
            iterator end() noexcept {
                iterator i; i._parent = _parent; i._size = _size; i._offset = ptrdiff_t(_size); return i;
            }

            const_iterator cbegin() const noexcept {
                const_iterator i; i._parent = _parent; i._size = _size; i._offset = 0i64; return i;
            }
            const_iterator cend() const noexcept {
                const_iterator i; i._parent = _parent; i._size = _size; i._offset = ptrdiff_t(_size); return i;
            }

            reverse_iterator rbegin() noexcept {
                reverse_iterator i; i._parent = _parent; i._size = _size; i._offset = ptrdiff_t(_size - 1i64); return i;
            }
            reverse_iterator rend() noexcept {
                reverse_iterator i; i._parent = _parent; i._size = _size; i._offset = -1i64; return i;
            }

            const_reverse_iterator crbegin() const noexcept {
                const_reverse_iterator i; i._parent = _parent; i._size = _size; i._offset = ptrdiff_t(_size - 1i64); return i;
            }
            const_reverse_iterator crend() const noexcept {
                const_reverse_iterator i; i._parent = _parent; i._size = _size; i._offset = -1i64; return i;
            }

            bounded_buffer_data(buffer* b, size_t size)
                : _parent(b), _size(size) {}

        private:
            buffer * _parent;
            size_t _size;
        };

        bounded_buffer_data iterate_mapped() noexcept 
        {
            if(_data)
                return bounded_buffer_data(this, _data_size);
            else
                return bounded_buffer_data(this, _size);
        }

        iterator begin() noexcept { return iterate_mapped().begin(); }
        iterator end() noexcept { return iterate_mapped().end(); }

        const_iterator cbegin() const noexcept { return iterate_mapped().cbegin(); }
        const_iterator cend() const noexcept { return iterate_mapped().cend(); }

        reverse_iterator rbegin() noexcept { return iterate_mapped().rbegin(); }
        reverse_iterator rend() noexcept { return iterate_mapped().rend(); }

        const_reverse_iterator crbegin() const noexcept { return iterate_mapped().crbegin(); }
        const_reverse_iterator crend() const noexcept { return iterate_mapped().crend(); }

        iterator erase(iterator it);
        template<typename It,
            typename = std::enable_if_t<std::is_same_v<typename std::iterator_traits<It>::value_type, T>>,
            typename = typename std::iterator_traits<It>::difference_type,
            typename = typename std::iterator_traits<It>::pointer,
            typename = typename std::iterator_traits<It>::reference,
            typename = typename std::iterator_traits<It>::value_type,
            typename = typename std::iterator_traits<It>::iterator_category
        >
        iterator insert(iterator at, It begin, It end);

    private:
        double compute_size_increase() const noexcept;
        void init_handle() noexcept;

        gl_buffer_t _id = gl_buffer_t::zero;
        T* _data = nullptr;
        mutable std::unique_ptr<T> _cached{ nullptr };
        size_t _cached_position = 0;
        size_t _data_size = 0;
        size_t _data_offset = 0;
        bool _data_full_size = false;
        GLbitfield _data_access = GL_ZERO;
        GLbitfield _usage = GL_ZERO;
        size_t _size = 0;
        size_t _reserved_size = 0;
        uint64_t _handle = 0;
    };
}

#include "buffer.inl"
#include "bounded_buffer_iterator.inl"