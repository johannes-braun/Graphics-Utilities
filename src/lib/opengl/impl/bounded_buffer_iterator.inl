#pragma once
#include <cassert>

namespace gl
{
    template<typename T, typename Ptr, int I> using bounded_buffer_iterator_base_impl = typename buffer<T>::bounded_buffer_iterator_base<Ptr, I>;

    template<typename T>
    template<typename Ptr, int Inc>
    bool bounded_buffer_iterator_base_impl<T, Ptr, Inc>::compatible(const bounded_buffer_iterator_base_impl<T, Ptr, Inc>& other) const noexcept
    {
        return _parent == other._parent &&
            _size == other._size;
    }

    template<typename T>
    template<typename Ptr, int Inc>
    bool bounded_buffer_iterator_base_impl<T, Ptr, Inc>::valid() const noexcept
    {
        return _parent && _size == _parent->_size;
    }

    template<typename T>
    template<typename Ptr, int Inc>
    bool bounded_buffer_iterator_base_impl<T, Ptr, Inc>::operator==(const bounded_buffer_iterator_base_impl<T, Ptr, Inc>& it) const noexcept
    {
        return valid() && it.valid() && compatible(it) && _offset == it._offset;
    }

    template<typename T>
    template<typename Ptr, int Inc>
    bool bounded_buffer_iterator_base_impl<T, Ptr, Inc>::operator!=(const bounded_buffer_iterator_base_impl<T, Ptr, Inc>& it) const noexcept
    {
        return !(*this == it);
    }

    template<typename T>
    template<typename Ptr, int Inc>
    bool bounded_buffer_iterator_base_impl<T, Ptr, Inc>::operator<(const bounded_buffer_iterator_base_impl<T, Ptr, Inc>& it) const noexcept
    {
        return valid() && it.valid() && compatible(it) && _offset < it._offset;
    }

    template<typename T>
    template<typename Ptr, int Inc>
    bool bounded_buffer_iterator_base_impl<T, Ptr, Inc>::operator>(const bounded_buffer_iterator_base_impl<T, Ptr, Inc>& it) const noexcept
    {
        return valid() && it.valid() && compatible(it) && _offset > it._offset;
    }

    template<typename T>
    template<typename Ptr, int Inc>
    bool bounded_buffer_iterator_base_impl<T, Ptr, Inc>::operator<=(const bounded_buffer_iterator_base_impl<T, Ptr, Inc>& it) const noexcept
    {
        return valid() && it.valid() && compatible(it) && _offset <= it._offset;
    }

    template<typename T>
    template<typename Ptr, int Inc>
    bool bounded_buffer_iterator_base_impl<T, Ptr, Inc>::operator>=(const bounded_buffer_iterator_base_impl<T, Ptr, Inc>& it) const noexcept
    {
        return valid() && it.valid() && compatible(it) && _offset >= it._offset;
    }

    template<typename T>
    template<typename Ptr, int Inc>
    bounded_buffer_iterator_base_impl<T, Ptr, Inc>& bounded_buffer_iterator_base_impl<T, Ptr, Inc>::operator+=(difference_type it) noexcept
    {
        return (*this = *this + it);
    }

    template<typename T>
    template<typename Ptr, int Inc>
    bounded_buffer_iterator_base_impl<T, Ptr, Inc>& bounded_buffer_iterator_base_impl<T, Ptr, Inc>::operator-=(difference_type it) noexcept
    {
        return (*this = *this - it);
    }

    template<typename T>
    template<typename Ptr, int Inc>
    bounded_buffer_iterator_base_impl<T, Ptr, Inc>& bounded_buffer_iterator_base_impl<T, Ptr, Inc>::operator--() noexcept
    {
        assert(valid() && "Iterator invalid");
        _offset -= Inc;
        return *this;
    }

    template<typename T>
    template<typename Ptr, int Inc>
    bounded_buffer_iterator_base_impl<T, Ptr, Inc>& bounded_buffer_iterator_base_impl<T, Ptr, Inc>::operator++() noexcept
    {
        assert(valid() && "Iterator invalid");
        _offset += Inc;
        return *this;
    }

    template<typename T, typename Ptr, int Inc>
    bounded_buffer_iterator_base_impl<T, Ptr, Inc> operator--(bounded_buffer_iterator_base_impl<T, Ptr, Inc>& rhs, int i) noexcept
    {
        bounded_buffer_iterator_base_impl<T, Ptr, Inc> new_it = rhs;
        --rhs;
        return new_it;
    }

    template<typename T, typename Ptr, int Inc>
    bounded_buffer_iterator_base_impl<T, Ptr, Inc> operator++(bounded_buffer_iterator_base_impl<T, Ptr, Inc>& rhs, int i) noexcept
    {
        bounded_buffer_iterator_base_impl<T, Ptr, Inc> new_it = rhs;
        ++rhs;
        return new_it;
    }

    template<typename T>
    template<typename Ptr, int Inc>
    bounded_buffer_iterator_base_impl<T, Ptr, Inc> bounded_buffer_iterator_base_impl<T, Ptr, Inc>::operator-(ptrdiff_t offset) const noexcept
    {
        assert(valid() && "Iterator invalid");
        bounded_buffer_iterator_base_impl<T, Ptr, Inc> new_it = *this;
        offset *= Inc;
        new_it._offset -= offset;
        return new_it;
    }

    template<typename T>
    template<typename Ptr, int Inc>
    bounded_buffer_iterator_base_impl<T, Ptr, Inc> bounded_buffer_iterator_base_impl<T, Ptr, Inc>::operator+(ptrdiff_t offset) const noexcept
    {
        assert(valid() && "Iterator invalid");
        bounded_buffer_iterator_base_impl<T, Ptr, Inc> new_it = *this;
        offset *= Inc;
        new_it._offset += offset;
        return new_it;
    }

    template<typename T>
    template<typename Ptr, int Inc>
    ptrdiff_t bounded_buffer_iterator_base_impl<T, Ptr, Inc>::operator-(const bounded_buffer_iterator_base_impl<T, Ptr, Inc>& offset) const noexcept
    {
        assert(compatible(offset) && "Iterators incompatible");
        assert(offset.valid() && "Iterator invalid");
        assert(valid() && "Iterator invalid");
        return _offset - offset._offset;
    }
    
    template<typename T>
    template<typename Ptr, int Inc>
    const Ptr bounded_buffer_iterator_base_impl<T, Ptr, Inc>::operator->() const noexcept
    {
        assert(valid() && "Iterator invalid");
        return &_parent->at(_offset);
    }

    template<typename T>
    template<typename Ptr, int Inc>
    std::enable_if_t<!std::is_const_v<Ptr>, Ptr> bounded_buffer_iterator_base_impl<T, Ptr, Inc>::operator->() noexcept
    {
        assert(valid() && "Iterator invalid");
        return &const_cast<buffer*>(_parent)->at(_offset);
    }

    template<typename T>
    template<typename Ptr, int Inc>
    const std::decay_t<std::remove_pointer_t<Ptr>>& bounded_buffer_iterator_base_impl<T, Ptr, Inc>::operator*() const noexcept
    {
        assert(valid() && "Iterator invalid");
        return _parent->at(_offset);
    }

    template<typename T>
    template<typename Ptr, int Inc>
    std::enable_if_t<!std::is_const_v<Ptr>, std::decay_t<std::remove_pointer_t<Ptr>>&> bounded_buffer_iterator_base_impl<T, Ptr, Inc>::operator*() noexcept
    {
        assert(valid() && "Iterator invalid");
        return _parent->at(_offset);
    }
}