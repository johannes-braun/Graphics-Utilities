#pragma once

namespace gl
{
    template<typename T>
    typename buffer<T>::iterator bounded_buffer_data<T>::begin() noexcept {
        typename buffer<T>::iterator i;
        i._parent = _parent;
        i._size = _size;
        i._offset = 0ll;
        return i;
    }

    template<typename T>
    typename buffer<T>::iterator bounded_buffer_data<T>::end() noexcept {
        typename buffer<T>::iterator i;
        i._parent = _parent;
        i._size = _size;
        i._offset = ptrdiff_t(_size);
        return i;
    }

    template<typename T>
    typename buffer<T>::const_iterator bounded_buffer_data<T>::begin() const noexcept {
        typename buffer<T>::const_iterator i;
        i._parent = _parent;
        i._size = _size;
        i._offset = 0ll;
        return i;
    }

    template<typename T>
    typename buffer<T>::const_iterator bounded_buffer_data<T>::end() const noexcept {
        typename buffer<T>::const_iterator i;
        i._parent = _parent;
        i._size = _size;
        i._offset = ptrdiff_t(_size);
        return i;
    }

    template<typename T>
    typename buffer<T>::const_iterator bounded_buffer_data<T>::cbegin() const noexcept {
        typename buffer<T>::const_iterator i;
        i._parent = _parent;
        i._size = _size;
        i._offset = 0ll;
        return i;
    }

    template<typename T>
    typename buffer<T>::const_iterator bounded_buffer_data<T>::cend() const noexcept {
        typename buffer<T>::const_iterator i;
        i._parent = _parent;
        i._size = _size;
        i._offset = ptrdiff_t(_size);
        return i;
    }

    template<typename T>
    typename buffer<T>::reverse_iterator bounded_buffer_data<T>::rbegin() noexcept {
        typename buffer<T>::reverse_iterator i;
        i._parent = _parent;
        i._size = _size;
        i._offset = ptrdiff_t(_size - 1ll);
        return i;
    }

    template<typename T>
    typename buffer<T>::reverse_iterator bounded_buffer_data<T>::rend() noexcept {
        typename buffer<T>::reverse_iterator i;
        i._parent = _parent;
        i._size = _size;
        i._offset = -1ll;
        return i;
    }

    template<typename T>
    typename buffer<T>::const_reverse_iterator bounded_buffer_data<T>::rbegin() const noexcept {
        typename buffer<T>::const_reverse_iterator i;
        i._parent = _parent;
        i._size = _size;
        i._offset = ptrdiff_t(_size - 1ll);
        return i;
    }
    template<typename T>
    typename buffer<T>::const_reverse_iterator bounded_buffer_data<T>::rend() const noexcept {
        typename buffer<T>::const_reverse_iterator i;
        i._parent = _parent;
        i._size = _size;
        i._offset = -1ll;
        return i;
    }

    template<typename T>
    typename buffer<T>::const_reverse_iterator bounded_buffer_data<T>::crbegin() const noexcept {
        typename buffer<T>::const_reverse_iterator i;
        i._parent = _parent;
        i._size = _size;
        i._offset = ptrdiff_t(_size - 1ll);
        return i;
    }
    template<typename T>
    typename buffer<T>::const_reverse_iterator bounded_buffer_data<T>::crend() const noexcept {
        typename buffer<T>::const_reverse_iterator i;
        i._parent = _parent;
        i._size = _size;
        i._offset = -1ll;
        return i;
    }

    template<typename T>
    bounded_buffer_data<T>::bounded_buffer_data(buffer<T>* b, size_t size)
        : _parent(b), _size(size) {}
}