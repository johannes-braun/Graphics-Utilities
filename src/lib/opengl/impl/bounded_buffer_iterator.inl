#pragma once

namespace gl
{
    //template<typename T, typename Ptr, int I> using bounded_buffer_iterator_base_impl = typename buffer<T>::bounded_buffer_iterator_base<Ptr, I>;

    template<typename T>
    template<typename Ptr, int I>
    bool typename buffer<T>::bounded_buffer_iterator_base<Ptr, I>::compatible(const typename buffer<T>::bounded_buffer_iterator_base<Ptr, I>& other) const noexcept
    {
        return _parent == other._parent &&
            _size == other._size;
    }

    template<typename T>
    template<typename Ptr, int Inc>
    bool typename buffer<T>::bounded_buffer_iterator_base<Ptr, Inc>::valid() const noexcept
    {
        return _parent && _size == _parent->_size;
    }

    template<typename T>
    template<typename Ptr, int Inc>
    bool typename buffer<T>::bounded_buffer_iterator_base<Ptr, Inc>::operator==(const typename buffer<T>::bounded_buffer_iterator_base<Ptr, Inc>& it) const noexcept
    {
        return valid() && it.valid() && compatible(it) && _offset == it._offset;
    }

    template<typename T>
    template<typename Ptr, int Inc>
    bool typename buffer<T>::bounded_buffer_iterator_base<Ptr, Inc>::operator!=(const typename buffer<T>::bounded_buffer_iterator_base<Ptr, Inc>& it) const noexcept
    {
        return !(*this == it);
    }

    template<typename T>
    template<typename Ptr, int Inc>
    bool typename buffer<T>::bounded_buffer_iterator_base<Ptr, Inc>::operator<(const typename buffer<T>::bounded_buffer_iterator_base<Ptr, Inc>& it) const noexcept
    {
        return valid() && it.valid() && compatible(it) && _offset < it._offset;
    }

    template<typename T>
    template<typename Ptr, int Inc>
    bool typename buffer<T>::bounded_buffer_iterator_base<Ptr, Inc>::operator>(const typename buffer<T>::bounded_buffer_iterator_base<Ptr, Inc>& it) const noexcept
    {
        return valid() && it.valid() && compatible(it) && _offset > it._offset;
    }

    template<typename T>
    template<typename Ptr, int Inc>
    bool typename buffer<T>::bounded_buffer_iterator_base<Ptr, Inc>::operator<=(const typename buffer<T>::bounded_buffer_iterator_base<Ptr, Inc>& it) const noexcept
    {
        return valid() && it.valid() && compatible(it) && _offset <= it._offset;
    }

    template<typename T>
    template<typename Ptr, int Inc>
    bool typename buffer<T>::bounded_buffer_iterator_base<Ptr, Inc>::operator>=(const typename buffer<T>::bounded_buffer_iterator_base<Ptr, Inc>& it) const noexcept
    {
        return valid() && it.valid() && compatible(it) && _offset >= it._offset;
    }

    template<typename T>
    template<typename Ptr, int Inc>
    typename buffer<T>::bounded_buffer_iterator_base<Ptr, Inc>& buffer<T>::bounded_buffer_iterator_base<Ptr, Inc>::operator+=(difference_type it) noexcept
    {
        return (*this = *this + it);
    }

    template<typename T>
    template<typename Ptr, int Inc>
    typename buffer<T>::bounded_buffer_iterator_base<Ptr, Inc>& buffer<T>::bounded_buffer_iterator_base<Ptr, Inc>::operator-=(difference_type it) noexcept
    {
        return (*this = *this - it);
    }

    template<typename T>
    template<typename Ptr, int Inc>
    typename buffer<T>::bounded_buffer_iterator_base<Ptr, Inc>& buffer<T>::bounded_buffer_iterator_base<Ptr, Inc>::operator--() noexcept
    {
        assert(valid() && "Iterator invalid");
        _offset -= Inc;
        return *this;
    }

    template<typename T>
    template<typename Ptr, int Inc>
    typename buffer<T>::bounded_buffer_iterator_base<Ptr, Inc>& buffer<T>::bounded_buffer_iterator_base<Ptr, Inc>::operator++() noexcept
    {
        assert(valid() && "Iterator invalid");
        _offset += Inc;
        return *this;
    }

    template<typename T>
    template<typename Ptr, int Inc>
    typename buffer<T>::bounded_buffer_iterator_base<Ptr, Inc> buffer<T>::bounded_buffer_iterator_base<Ptr, Inc>::operator-(ptrdiff_t offset) const noexcept
    {
        assert(valid() && "Iterator invalid");
        auto new_it = *this;
        offset *= Inc;
        new_it._offset -= offset;
        return new_it;
    }

    template<typename T>
    template<typename Ptr, int Inc>
    typename buffer<T>::bounded_buffer_iterator_base<Ptr, Inc> buffer<T>::bounded_buffer_iterator_base<Ptr, Inc>::operator+(ptrdiff_t offset) const noexcept
    {
        assert(valid() && "Iterator invalid");
        auto new_it = *this;
        offset *= Inc;
        new_it._offset += offset;
        return new_it;
    }

    template<typename T>
    template<typename Ptr, int Inc>
    ptrdiff_t buffer<T>::bounded_buffer_iterator_base<Ptr, Inc>::operator-(const typename buffer<T>::bounded_buffer_iterator_base<Ptr, Inc>& offset) const noexcept
    {
        assert(compatible(offset) && "Iterators incompatible");
        assert(offset.valid() && "Iterator invalid");
        assert(valid() && "Iterator invalid");
        return _offset - offset._offset;
    }
    
    template<typename T>
    template<typename Ptr, int Inc>
    const Ptr buffer<T>::bounded_buffer_iterator_base<Ptr, Inc>::operator->() const noexcept
    {
        assert(valid() && "Iterator invalid");
        return &_parent->at(_offset);
    }

    template<typename T>
    template<typename Ptr, int Inc>
    std::enable_if_t<!std::is_const_v<Ptr>, Ptr> buffer<T>::bounded_buffer_iterator_base<Ptr, Inc>::operator->() noexcept
    {
        assert(valid() && "Iterator invalid");
        return &const_cast<buffer*>(_parent)->at(_offset);
    }

    template<typename T>
    template<typename Ptr, int Inc>
    const std::decay_t<std::remove_pointer_t<Ptr>>& buffer<T>::bounded_buffer_iterator_base<Ptr, Inc>::operator*() const noexcept
    {
        assert(valid() && "Iterator invalid");
        return _parent->at(_offset);
    }

    template<typename T>
    template<typename Ptr, int Inc>
    std::enable_if_t<!std::is_const_v<Ptr>, std::decay_t<std::remove_pointer_t<Ptr>>&> buffer<T>::bounded_buffer_iterator_base<Ptr, Inc>::operator*() noexcept
    {
        assert(valid() && "Iterator invalid");
        return _parent->at(_offset);
    }
}

template<typename T, typename Ptr, int Inc>
typename gl::buffer<T>::bounded_buffer_iterator_base<Ptr, Inc> operator--(typename gl::buffer<T>::bounded_buffer_iterator_base<Ptr, Inc>& rhs, int i) noexcept
{
    auto new_it = rhs;
    --rhs;
    return new_it;
}

template<typename T, typename Ptr, int Inc>
typename gl::buffer<T>::bounded_buffer_iterator_base<Ptr, Inc> operator++(typename gl::buffer<T>::bounded_buffer_iterator_base<Ptr, Inc>& rhs, int i) noexcept
{
    auto new_it = rhs;
    ++rhs;
    return new_it;
}