#pragma once

namespace gfx
{
template <typename T>
device_buffer<T>::device_buffer(const buffer_usage_flags usage)
        : _implementation(detail::make_device_buffer_implementation()), _usage_flags(usage)
{
    _implementation->update_flags(usage);
}

template <typename T>
device_buffer<T>::device_buffer(const buffer_usage_flags usage, const size_type elements)
        : device_buffer(usage)
{
    _implementation->allocate(elements * type_size);
    _size = elements;
}

template <typename T>
device_buffer<T>::device_buffer(const size_type elements)
        : device_buffer(buffer_usage::all, elements)
{
}

template <typename T> void device_buffer<T>::reallocate(const size_type elements)
{
    _implementation->allocate(elements * type_size);
    _size = elements;
}

template <typename T> buffer_usage_flags device_buffer<T>::usage() const noexcept { return _usage_flags; }
template <typename T> typename device_buffer<T>::size_type device_buffer<T>::capacity() const noexcept { return _size; }

template <typename T> void device_buffer<T>::operator>>(const host_buffer<T>& buffer)
{
    copy_to(buffer, 0, 0, std::min<size_type>(buffer.size(), capacity()));
}

template <typename T> void device_buffer<T>::operator<<(const host_buffer<T>& buffer)
{
    fill_from(buffer, 0, 0, std::min<size_type>(buffer.size(), capacity()));
}

template <typename T> void device_buffer<T>::operator>>(const device_buffer& buffer)
{
    copy_to(buffer, 0, 0, std::min<size_type>(buffer.capacity(), capacity()));
}

template <typename T> void device_buffer<T>::operator<<(const device_buffer& buffer)
{
    fill_from(buffer, 0, 0, std::min<size_type>(buffer.capacity(), capacity()));
}

template <typename T> void device_buffer<T>::update(const T* data, size_type count, const difference_type start) const
{
    if(start + count > _size)
        throw std::out_of_range("Destination out of range.");
    _implementation->update(start * type_size, count * type_size, reinterpret_cast<const std::byte*>(data));
}

template <typename T> void device_buffer<T>::update(const std::initializer_list<T>& elements, const difference_type start) const
{
    update(std::data(elements), std::size(elements), start);
}

template <typename T>
template <typename Container, typename>
void device_buffer<T>::update(const Container& elements, const difference_type start) const
{
    update(std::data(elements), std::size(elements), start);
}

template <typename T>
void device_buffer<T>::fill_from(const host_buffer<T>& buffer, const difference_type src_offset, const difference_type start,
                                 const size_type count)
{
    if(start + count > _size)
        throw std::out_of_range("Destination out of range.");
    if(src_offset + count > buffer.size())
        throw std::out_of_range("Source out of range.");

    _implementation->copy(&*buffer._implementation, &*_implementation, src_offset * type_size, start * type_size, count * type_size);
}

//template <typename T>
//template <typename H> 
//H device_buffer<T>::api_handle() const
//{
//    if constexpr(std::is_same_v<H, std::any>)
//        return _implementation->api_handle();
//    else
//        return std::any_cast<H>(_implementation->api_handle());
//}

template <typename T>
void device_buffer<T>::fill_from(const device_buffer& buffer, const difference_type src_offset, const difference_type start,
                                 const size_type count)
{
    if(start + count > _size)
        throw std::out_of_range("Destination out of range.");
    if(src_offset + count > buffer.capacity())
        throw std::out_of_range("Source out of range.");

    _implementation->copy(&*buffer._implementation, &*_implementation, src_offset * type_size, start * type_size, count * type_size);
}

template <typename T>
void device_buffer<T>::copy_to(const host_buffer<T>& buffer, const difference_type src_offset, const difference_type dst_offset,
                               const size_type count)
{
    if(dst_offset + count > _size)
        throw std::out_of_range("Destination out of range.");
    if(src_offset + count > buffer.size())
        throw std::out_of_range("Source out of range.");

    _implementation->copy(&*_implementation, &*buffer._implementation, src_offset * type_size, dst_offset * type_size, count * type_size);
}

template <typename T>
void device_buffer<T>::copy_to(const device_buffer& buffer, const difference_type src_offset, const difference_type dst_offset,
                               const size_type count)
{
    if(dst_offset + count > _size)
        throw std::out_of_range("Destination out of range.");
    if(src_offset + count > buffer.capacity())
        throw std::out_of_range("Source out of range.");

    _implementation->copy(&*_implementation, &*buffer._implementation, src_offset * type_size, dst_offset * type_size, count * type_size);
}
}