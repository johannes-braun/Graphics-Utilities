#pragma once

namespace gl::v2
{
    template<typename T> using bounded_buffer_data_impl = typename buffer<T>::bounded_buffer_data;

    template<typename T>
    typename buffer<T>::iterator bounded_buffer_data_impl<T>::begin() noexcept {
        iterator i;
        i._parent = _parent;
        i._size = _size;
        i._offset = 0i64;
        return i;
    }

    template<typename T>
    typename buffer<T>::iterator bounded_buffer_data_impl<T>::end() noexcept {
        iterator i;
        i._parent = _parent;
        i._size = _size;
        i._offset = ptrdiff_t(_size);
        return i;
    }

    template<typename T>
    typename buffer<T>::const_iterator bounded_buffer_data_impl<T>::cbegin() const noexcept {
        const_iterator i;
        i._parent = _parent;
        i._size = _size;
        i._offset = 0i64;
        return i;
    }

    template<typename T>
    typename buffer<T>::const_iterator bounded_buffer_data_impl<T>::cend() const noexcept {
        const_iterator i;
        i._parent = _parent;
        i._size = _size;
        i._offset = ptrdiff_t(_size);
        return i;
    }

    template<typename T>
    typename buffer<T>::reverse_iterator bounded_buffer_data_impl<T>::rbegin() noexcept {
        reverse_iterator i;
        i._parent = _parent;
        i._size = _size;
        i._offset = ptrdiff_t(_size - 1i64);
        return i;
    }

    template<typename T>
    typename buffer<T>::reverse_iterator bounded_buffer_data_impl<T>::rend() noexcept {
        reverse_iterator i;
        i._parent = _parent;
        i._size = _size;
        i._offset = -1i64;
        return i;
    }

    template<typename T>
    typename buffer<T>::const_reverse_iterator bounded_buffer_data_impl<T>::crbegin() const noexcept {
        const_reverse_iterator i;
        i._parent = _parent;
        i._size = _size;
        i._offset = ptrdiff_t(_size - 1i64);
        return i;
    }
    template<typename T>
    typename buffer<T>::const_reverse_iterator bounded_buffer_data_impl<T>::crend() const noexcept {
        const_reverse_iterator i;
        i._parent = _parent;
        i._size = _size;
        i._offset = -1i64;
        return i;
    }

    template<typename T>
    buffer<T>::bounded_buffer_data::bounded_buffer_data(buffer<T>* b, size_t size)
        : _parent(b), _size(size) {}
}