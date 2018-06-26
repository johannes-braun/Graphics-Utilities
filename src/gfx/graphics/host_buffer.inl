#pragma once

namespace gfx
{
template <typename T> template <bool E, typename> void host_buffer<T>::resize(const size_type elements) { resize(elements, value_type{}); }

template <typename T> void host_buffer<T>::resize(size_type elements, const_reference base)
{
    if(_size != elements)
    {
        _data = type_ptr(_implementation->grow(byte_ptr(_data), _size * type_size, elements * type_size));
        if(_size < elements)
            std::generate(_data + _size, _data + elements, [&]() { return base; });
        _size = elements;
    }
}

template <typename T>
host_buffer<T>::host_buffer()
        : _implementation(detail::make_host_buffer_implementation())
        , _size(0)
        , _data(nullptr)
{
}

template <typename T>
template <bool E, typename>
host_buffer<T>::host_buffer(const size_type elements)
        : host_buffer(elements, value_type{})
{
}

template <typename T>
host_buffer<T>::host_buffer(const size_type elements, const_reference base)
        : _implementation(detail::make_host_buffer_implementation())
        , _size(elements)
        , _data(type_ptr(_implementation->grow(nullptr, 0, _size * type_size)))
{
    std::generate(std::execution::par_unseq, _data, _data + _size, [&]() { return base; });
}

template <typename T>
template <typename Iter>
host_buffer<T>::host_buffer(Iter begin, Iter end)
        : _implementation(detail::make_host_buffer_implementation())
        , _size(std::distance(begin, end))
        , _data(type_ptr(_implementation->grow(nullptr, 0, _size * type_size)))
{
    std::copy(std::execution::par_unseq, begin, end, _data);
}

template <typename T>
host_buffer<T>::host_buffer(const std::initializer_list<T>& elements)
        : host_buffer(std::data(elements), std::data(elements) + std::size(elements))
{
}

template <typename T>
template <typename Container, typename>
host_buffer<T>::host_buffer(const Container& elements)
        : host_buffer(std::data(elements), std::data(elements) + std::size(elements))
{
}
template <typename T> typename host_buffer<T>::reference host_buffer<T>::at(const size_type index)
{
    if(index >= _size)
        throw std::out_of_range("Buffer index out of range.");
    return _data[index];
}
template <typename T> typename host_buffer<T>::const_reference host_buffer<T>::at(const size_type index) const
{
    if(index >= _size)
        throw std::out_of_range("Buffer index out of range.");
    return _data[index];
}
template <typename T> typename host_buffer<T>::reference host_buffer<T>::operator[](const size_type index) { return at(index); }
template <typename T> typename host_buffer<T>::const_reference host_buffer<T>::operator[](const size_type index) const { return at(index); }
template <typename T> typename host_buffer<T>::size_type                       host_buffer<T>::size() const noexcept { return _size; }
template <typename T> bool                                                     host_buffer<T>::empty() const noexcept { return _size == 0; }
template <typename T> typename host_buffer<T>::iterator                        host_buffer<T>::find(const_reference object) noexcept
{
    return std::find(begin(), end(), object);
}
template <typename T> typename host_buffer<T>::const_iterator host_buffer<T>::find(const_reference object) const noexcept
{
    return std::find(begin(), end(), object);
}
template <typename T> bool             host_buffer<T>::contains(const_reference object) const noexcept { return find(object) != end(); }
template <typename T> const std::byte* host_buffer<T>::byte_ptr(const const_pointer ptr) { return reinterpret_cast<const std::byte*>(ptr); }
template <typename T> std::byte*       host_buffer<T>::byte_ptr(const pointer ptr) { return reinterpret_cast<std::byte*>(ptr); }
template <typename T> typename host_buffer<T>::const_pointer host_buffer<T>::type_ptr(const std::byte* ptr)
{
    return reinterpret_cast<const_pointer>(ptr);
}
template <typename T> typename host_buffer<T>::pointer host_buffer<T>::type_ptr(std::byte* ptr) { return reinterpret_cast<pointer>(ptr); }
}