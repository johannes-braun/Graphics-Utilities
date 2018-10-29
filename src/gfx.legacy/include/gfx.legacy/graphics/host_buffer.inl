#pragma once

namespace gfx {
inline namespace v1 {
template<typename T>
template<typename>
void host_buffer<T>::resize(const size_type elements)
{
    resize(elements, value_type{});
}

template<typename T>
void host_buffer<T>::resize(size_type elements, const_reference base)
{
    if (_data_span.size() != elements) {
        const auto last_size = _data_span.size();

        if (elements < _data_span.size()) {
            std::for_each(_data_span.begin() + elements, _data_span.end(), [](value_type& it) { it.~value_type(); });
			_data_span  ={ _data_span.data(), elements };
        }
		else
		{
			auto    new_alloc = implementation()->allocate(elements * type_size);
			pointer ptr       = static_cast<pointer>(new_alloc.data);

			for (size_type i = 0; i < elements; ++i) {
				if (i < _data_span.size())
					ptr[i] = std::move(_data_span[i]);
				else
					ptr[i] = base;
			}
			implementation()->deallocate(_allocation);
			_allocation = new_alloc;
			_data_span  ={ ptr, elements };
			_capacity   = elements;
		}
    }
}

template<typename T>
void host_buffer<T>::reserve(size_type size)
{
    if (_capacity < size) {
        auto    new_alloc = implementation()->allocate(size * type_size);
        pointer ptr       = static_cast<pointer>(new_alloc.data);

        for (size_type i = 0; i < _data_span.size(); ++i) {
            ptr[i] = std::move(_data_span[i]);
        }
        implementation()->deallocate(_allocation);
        _allocation = new_alloc;
        _data_span  = {ptr, _data_span.size()};
        _capacity   = size;
    }
}

template<typename T>
host_buffer<T>::host_buffer() : _data_span(static_cast<value_type*>(nullptr), 0ll)
{}

template<typename T>
template<typename>
host_buffer<T>::host_buffer(const size_type elements) : host_buffer(elements, value_type{})
{}

template<typename T>
host_buffer<T>::host_buffer(const size_type elements, const_reference base)
      : _allocation(implementation()->allocate(elements * type_size)), _data_span(static_cast<pointer>(_allocation.data), elements)
{
    std::generate(std::execution::par_unseq, begin(), end(), [&]() { return base; });
}

template<typename T>
template<typename Iter>
host_buffer<T>::host_buffer(Iter begin, Iter end)
{
    const auto dist = std::distance(begin, end);
    _allocation     = implementation()->allocate(dist * type_size);
    _data_span      = {static_cast<pointer>(_allocation.data), dist};
    std::copy(begin, end, _data_span.begin());
}

template<typename T>
host_buffer<T>::host_buffer(const std::initializer_list<T>& elements)
      : host_buffer(std::data(elements), std::data(elements) + std::size(elements))
{}

template<typename T>
template<typename Container, typename>
host_buffer<T>::host_buffer(const Container& elements) : host_buffer(std::data(elements), std::data(elements) + std::size(elements))
{}
template<typename T>
typename host_buffer<T>::reference host_buffer<T>::at(const size_type index)
{
    if (index >= _data_span.size()) throw std::out_of_range("Buffer index out of range.");
    return _data_span[index];
}
template<typename T>
typename host_buffer<T>::const_reference host_buffer<T>::at(const size_type index) const
{
    if (index >= _data_span.size()) throw std::out_of_range("Buffer index out of range.");
    return _data_span[index];
}
template<typename T>
typename host_buffer<T>::reference host_buffer<T>::operator[](const size_type index)
{
    return at(index);
}
template<typename T>
typename host_buffer<T>::const_reference host_buffer<T>::operator[](const size_type index) const
{
    return at(index);
}
template<typename T>
typename host_buffer<T>::size_type host_buffer<T>::size() const noexcept
{
    return _data_span.size();
}

template<typename T>
typename host_buffer<T>::size_type host_buffer<T>::capacity() const noexcept
{
    return _capacity;
}

template<typename T>
bool host_buffer<T>::empty() const noexcept
{
    return _data_span.empty();
}
template<typename T>
typename host_buffer<T>::pointer host_buffer<T>::data() noexcept
{
    return _data_span.data();
}
template<typename T>
typename host_buffer<T>::const_pointer host_buffer<T>::data() const noexcept
{
    return _data_span.data();
}

template<typename T>
typename host_buffer<T>::iterator host_buffer<T>::begin() noexcept
{
    return _data_span.begin();
}

template<typename T>
typename host_buffer<T>::iterator host_buffer<T>::end() noexcept
{
    return _data_span.end();
}

template<typename T>
typename host_buffer<T>::const_iterator host_buffer<T>::begin() const noexcept
{
    return _data_span.begin();
}

template<typename T>
typename host_buffer<T>::const_iterator host_buffer<T>::end() const noexcept
{
    return _data_span.end();
}

template<typename T>
typename host_buffer<T>::const_iterator host_buffer<T>::cbegin() const noexcept
{
    return _data_span.cbegin();
}

template<typename T>
typename host_buffer<T>::const_iterator host_buffer<T>::cend() const noexcept
{
    return _data_span.cend();
}

template<typename T>
typename host_buffer<T>::reverse_iterator host_buffer<T>::rbegin() noexcept
{
    return _data_span.rbegin();
}

template<typename T>
typename host_buffer<T>::reverse_iterator host_buffer<T>::rend() noexcept
{
    return _data_span.rend();
}

template<typename T>
typename host_buffer<T>::const_reverse_iterator host_buffer<T>::rbegin() const noexcept
{
    return _data_span.rbegin();
}

template<typename T>
typename host_buffer<T>::const_reverse_iterator host_buffer<T>::rend() const noexcept
{
    return _data_span.rend();
}

template<typename T>
typename host_buffer<T>::const_reverse_iterator host_buffer<T>::crbegin() const noexcept
{
    return _data_span.crend();
}

template<typename T>
typename host_buffer<T>::const_reverse_iterator host_buffer<T>::crend() const noexcept
{
    return _data_span.crend();
}

template<typename T>
typename host_buffer<T>::iterator host_buffer<T>::find(const_reference object) noexcept
{
    return std::find(begin(), end(), object);
}
template<typename T>
typename host_buffer<T>::const_iterator host_buffer<T>::find(const_reference object) const noexcept
{
    return std::find(begin(), end(), object);
}
template<typename T>
bool host_buffer<T>::contains(const_reference object) const noexcept
{
    return find(object) != end();
}

template<typename T>
void host_buffer<T>::shrink_to_fit()
{
    if (_capacity != _data_span.size()) {
        resize(_data_span.size());
    }
}

template<typename T>
void host_buffer<T>::push_back(value_type&& value)
{
    if (_data_span.size() < _capacity) {
        (_data_span = {_data_span.data(), _data_span.size() + 1})[_data_span.size() - 1] = std::forward<value_type&&>(value);
        return;
    }

    reserve(std::max(_capacity * 2, 1ll));
    (_data_span = {_data_span.data(), _data_span.size() + 1})[_data_span.size() - 1] = std::forward<value_type&&>(value);
}

template<typename T>
void host_buffer<T>::push_back(const value_type& value)
{
	if (_data_span.size() < _capacity) {
		(_data_span = {_data_span.data(), _data_span.size() + 1})[_data_span.size() - 1] = value;
		return;
	}

	reserve(std::max(_capacity * 2, 1ll));
	(_data_span = {_data_span.data(), _data_span.size() + 1})[_data_span.size() - 1] = value;
}

template<typename T>
template<typename... Args, typename>
T& host_buffer<T>::emplace_back(Args&&... args)
{
    if (_data_span.size() < _capacity)
        return *new (&(_data_span = {_data_span.data(), _data_span.size() + 1})[_data_span.size() - 1])
            value_type(std::forward<Args&&>(args)...);
    reserve(std::max(_capacity * 2, 1ll));
    return *new (&(_data_span = {_data_span.data(), _data_span.size() + 1})[_data_span.size() - 1])
        value_type(std::forward<Args&&>(args)...);
}

template<typename T>
const std::byte* host_buffer<T>::byte_ptr(const const_pointer ptr)
{
    return reinterpret_cast<const std::byte*>(ptr);
}
template<typename T>
std::byte* host_buffer<T>::byte_ptr(const pointer ptr)
{
    return reinterpret_cast<std::byte*>(ptr);
}
template<typename T>
typename host_buffer<T>::const_pointer host_buffer<T>::type_ptr(const std::byte* ptr)
{
    return reinterpret_cast<const_pointer>(ptr);
}
template<typename T>
typename host_buffer<T>::pointer host_buffer<T>::type_ptr(std::byte* ptr)
{
    return reinterpret_cast<pointer>(ptr);
}
}    // namespace v1
}    // namespace gfx