#pragma once

namespace gfx {
inline namespace v1 {
template<typename T>
device_buffer<T>::device_buffer(const buffer_usage_flags usage) : _usage_flags(usage)
{
    implementation()->update_flags(usage);
}

template<typename T>
device_buffer<T>::device_buffer(const buffer_usage_flags usage, const size_type elements) : device_buffer(usage)
{
    implementation()->allocate(elements * type_size);
    _size = elements;
}

template<typename T>
device_buffer<T>::device_buffer(buffer_usage_flags usage, const host_buffer<T>& source) : device_buffer(usage, source.size())
{
    *this << source;
}

template<typename T>
device_buffer<T>::device_buffer(const size_type elements) : device_buffer(buffer_usage::all, elements)
{}

template<typename T>
void device_buffer<T>::reallocate(const size_type elements)
{
    implementation()->allocate(elements * type_size);
    _size = elements;
}

template<typename T>
buffer_usage_flags device_buffer<T>::usage() const noexcept
{
    return _usage_flags;
}
template<typename T>
typename device_buffer<T>::size_type device_buffer<T>::capacity() const noexcept
{
    return _size;
}

template<typename T>
typename device_buffer<T>::size_type device_buffer<T>::size() const noexcept
{
    return capacity();
}

template<typename T>
void device_buffer<T>::operator>>(const host_buffer<T>& buffer) const
{
    copy_to(buffer, 0, 0, std::min<size_type>(buffer.size(), capacity()));
}

template<typename T>
void device_buffer<T>::operator<<(const host_buffer<T>& buffer)
{
    fill_from(buffer, 0, 0, std::min<size_type>(buffer.size(), capacity()));
}

template<typename T>
void device_buffer<T>::operator>>(const device_buffer& buffer) const
{
    copy_to(buffer, 0, 0, std::min<size_type>(buffer.capacity(), capacity()));
}

template<typename T>
void device_buffer<T>::operator<<(const device_buffer& buffer)
{
    fill_from(buffer, 0, 0, std::min<size_type>(buffer.capacity(), capacity()));
}

template<typename T>
void device_buffer<T>::update(const T* data, size_type count, const difference_type start) const
{
    if (start + count > _size) throw std::out_of_range("Destination out of range.");
    implementation()->update(start * type_size, count * type_size, reinterpret_cast<const std::byte*>(data));
}

template<typename T>
void device_buffer<T>::update(const std::initializer_list<T>& elements, const difference_type start) const
{
    update(std::data(elements), std::size(elements), start);
}

template<typename T>
template<typename Container, typename>
void device_buffer<T>::update(const Container& elements, const difference_type start) const
{
    update(std::data(elements), std::size(elements), start);
}

template<typename T>
void device_buffer<T>::fill_from(const host_buffer<T>& buffer, const difference_type src_offset, const difference_type start,
                                 const size_type count, fence* f)
{
    if (start + count > _size) throw std::out_of_range("Destination out of range.");
    if (src_offset + count > buffer.size()) throw std::out_of_range("Source out of range.");

    implementation()->copy(&*buffer.implementation(), &*implementation(), src_offset * type_size, start * type_size, count * type_size, f);
}

// template <typename T>
// template <typename H>
// H device_buffer<T>::api_handle() const
//{
//    if constexpr(std::is_same_v<H, std::any>)
//        return implementation()->api_handle();
//    else
//        return std::any_cast<H>(implementation()->api_handle());
//}
template<typename T>
host_buffer<T> device_buffer<T>::to_host() const
{
    host_buffer<T> result(_size);
    *this >> result;
    return result;
}

template<typename T>
void device_buffer<T>::fill_from(const device_buffer& buffer, const difference_type src_offset, const difference_type start,
                                 const size_type count, fence* f)
{
    if (start + count > _size) throw std::out_of_range("Destination out of range.");
    if (src_offset + count > buffer.capacity()) throw std::out_of_range("Source out of range.");

    implementation()->copy(&*buffer.implementation(), &*implementation(), src_offset * type_size, start * type_size, count * type_size, f);
}

template<typename T>
void device_buffer<T>::copy_to(const host_buffer<T>& buffer, const difference_type src_offset, const difference_type dst_offset,
                               const size_type count, fence* f) const
{
    if (dst_offset + count > _size) throw std::out_of_range("Destination out of range.");
    if (src_offset + count > buffer.size()) throw std::out_of_range("Source out of range.");

    implementation()->copy(&*implementation(), &*buffer.implementation(), src_offset * type_size, dst_offset * type_size,
                           count * type_size, f);
}

template<typename T>
void device_buffer<T>::copy_to(const device_buffer& buffer, const difference_type src_offset, const difference_type dst_offset,
                               const size_type count, fence* f) const
{
    if (dst_offset + count > _size) throw std::out_of_range("Destination out of range.");
    if (src_offset + count > buffer.capacity()) throw std::out_of_range("Source out of range.");

    implementation()->copy(&*implementation(), &*buffer.implementation(), src_offset * type_size, dst_offset * type_size,
                           count * type_size, f);
}


template<typename BufDst, typename BufSrc, typename>
void buf_copy(buf_copy_behavior behavior, BufDst& dst, BufSrc& src, size_t count, ptrdiff_t src_offset, ptrdiff_t dst_offset, fence* f)
{
	constexpr static const char* invalid_argument_message = "Selected copy range does not fit into destination buffer.";

	if constexpr (is_host_buffer<BufSrc> && is_device_buffer<BufDst>)
	{
		switch(behavior)
		{
		case buf_copy_behavior::resize_if_larger: 
			if (dst.capacity() + dst_offset < src.size() + src_offset)
				dst.reallocate(count + dst_offset);
			break;
		case buf_copy_behavior::resize_always: 
			dst.reallocate(count + dst_offset);
			break;
		case buf_copy_behavior::throw_if_larger:
			if (dst.capacity() + dst_offset < src.size() + src_offset)
				throw std::invalid_argument(invalid_argument_message);
			break;
		}
		dst.fill_from(src, src_offset, dst_offset, count, f);
	}
	else if constexpr (is_device_buffer<BufSrc> && is_device_buffer<BufDst>)
	{
		switch(behavior)
		{
		case buf_copy_behavior::resize_if_larger: 
			if (dst.capacity() + dst_offset < src.capacity() + src_offset)
				dst.reallocate(count + dst_offset);
			break;
		case buf_copy_behavior::resize_always: 
			dst.reallocate(count + dst_offset);
			break;
		case buf_copy_behavior::throw_if_larger:
			if (dst.capacity() + dst_offset < src.capacity() + src_offset)
				throw std::invalid_argument(invalid_argument_message);
			break;
		}
		src.copy_to(dst, src_offset, dst_offset, count, f);
	}
	else if constexpr (is_device_buffer<BufSrc> && is_host_buffer<BufDst>)
	{
		switch(behavior)
		{
		case buf_copy_behavior::resize_if_larger: 
			if (dst.size() + dst_offset < src.capacity() + src_offset)
				dst.resize(count + dst_offset);
			break;
		case buf_copy_behavior::resize_always: 
			dst.resize(count + dst_offset);
			break;
		case buf_copy_behavior::throw_if_larger:
			if (dst.size() + dst_offset < src.capacity() + src_offset)
				throw std::invalid_argument(invalid_argument_message);
			break;
		}
		src.copy_to(dst, src_offset, dst_offset, count, f);
	}
}

template<typename BufDst, typename BufSrc, typename>
void buf_copy(BufDst& dst, BufSrc& src, size_t count, ptrdiff_t src_offset, ptrdiff_t dst_offset, fence* f)
{
	buf_copy(buf_copy_behavior::resize_if_larger, dst, src, count, src_offset, dst_offset, f);
}
template<typename BufDst, typename BufSrc, typename>
void buf_copy(buf_copy_behavior behavior, BufDst& dst, BufSrc& src, size_t count, fence* f)
{
	buf_copy(behavior, dst, src, count, 0, 0, f);
}

template<typename BufDst, typename BufSrc, typename>
void buf_copy(BufDst& dst, BufSrc& src, size_t count, fence* f)
{
	buf_copy(buf_copy_behavior::resize_if_larger, dst, src, count, f);
}
}    // namespace v1
}    // namespace gfx