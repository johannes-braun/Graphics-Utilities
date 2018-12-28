#pragma once

namespace gfx {
inline namespace v1 {
namespace opengl {
template<typename T>
mapped<T>::mapped(size_type size, T&& value)
{
    resize(size, std::forward<T&&>(value));
}
template<typename T>
mapped<T>::mapped(std::initializer_list<T> ilist)
{
    reserve(std::size(ilist));
    reset_storage(this->data(), std::size(ilist));
    std::move(std::begin(ilist), std::end(ilist), this->begin());
}
template<typename T>
mapped<T>::mapped(gsl::span<T const> ilist)
{
    reserve(std::size(ilist));
    reset_storage(this->data(), std::size(ilist));
    std::copy(std::begin(ilist), std::end(ilist), this->begin());
}
template<typename T>
mapped<T>::mapped(mapped const& other)
{
    allocate(other._capacity);
    reset_storage(this->data(), other.size());
    std::copy(other.begin(), other.end(), this->begin());
}
template<typename T>
mapped<T>::mapped(mapped&& other) noexcept
{
    reset_storage(other.data(), other.size());
    _buffer   = std::move(other._buffer);
    _capacity = other._capacity;

    other.reset_storage(nullptr, 0);
    other._buffer   = mygl::buffer::zero();
    other._capacity = 0;
}
template<typename T>
mapped<T>& mapped<T>::operator=(mapped<T> const& other)
{
    if constexpr (!std::is_trivially_destructible_v<T>) std::destroy(this->begin(), this->end());
    allocate(other._capacity);
    reset_storage(this->data(), other.size());
    std::copy(other.begin(), other.end(), this->begin());
    return *this;
}
template<typename T>
mapped<T>& mapped<T>::operator=(mapped<T>&& other) noexcept
{
    if constexpr (!std::is_trivially_destructible_v<T>) std::destroy(this->begin(), this->end());
    reset_storage(other.data(), other.size());
    _buffer   = std::move(other._buffer);
    _capacity = other._capacity;

    other.reset_storage(nullptr, 0);
    other._buffer   = mygl::buffer::zero();
    other._capacity = 0;
    return *this;
}
template<typename T>
mapped<T>::~mapped()
{
    if constexpr (!std::is_trivially_destructible_v<T>) std::destroy(this->begin(), this->end());
    if (glIsBuffer(_buffer)) glDeleteBuffers(1, &_buffer);
}
template<typename T>
void mapped<T>::reserve(size_type capacity)
{
    allocate(capacity);
}
template<typename T>
void mapped<T>::resize(size_type size, T&& value)
{
    if constexpr (!std::is_trivially_destructible_v<T>)
        if (size < this->size()) std::destroy(this->begin() + size, this->end());
    while (size > _capacity) allocate(std::max(2 * _capacity, 1ll));
    auto const old_size = this->size();

    reset_storage(this->data(), size);
    if (size > old_size) init_range(this->begin() + old_size, this->end(), std::forward<T&&>(value));
}
template<typename T>
void mapped<T>::resize(size_type size, T const& value)
{
    if constexpr (!std::is_trivially_destructible_v<T>)
        if (size < this->size()) std::destroy(this->begin() + size, this->end());
    while (size > _capacity) allocate(std::max(2 * _capacity, 1ll));
    auto const old_size = this->size();
    reset_storage(this->data(), size);
    if (size > old_size) init_range(this->begin() + old_size, this->end(), std::forward<const T&>(value));
}
template<typename T>
void mapped<T>::push_back(T&& value)
{
    resize(this->size() + 1, std::forward<T&&>(value));
}
template<typename T>
void mapped<T>::push_back(T const& value)
{
    resize(this->size() + 1, std::forward<const T&>(value));
}
template<typename T>
void mapped<T>::pop_back()
{
    if constexpr (!std::is_trivially_destructible_v<T>) std::destroy_at(std::addressof(back()));
    reset_storage(this->data(), this->size() - 1);
}
template<typename T>
typename mapped<T>::iterator mapped<T>::insert(const_iterator at, std::initializer_list<T> ilist)
{
    return insert(at, std::begin(ilist), std::end(ilist));
}
template<typename T>
typename mapped<T>::iterator mapped<T>::erase(const_iterator at)
{
    auto const start_offset = std::distance<const_iterator>(this->begin(), at);
    auto       result_it    = this->begin() + start_offset;
    if constexpr (!std::is_trivially_destructible_v<T>) std::destroy_at(std::addressof(*result_it));
    std::move(std::next(result_it), this->end(), result_it);
    reset_storage(this->data(), this->size() - 1);
    return result_it;
}
template<typename T>
typename mapped<T>::iterator mapped<T>::erase(const_iterator begin, const_iterator end)
{
    auto const start_offset = std::distance<const_iterator>(this->begin(), begin);
    auto const delta        = std::distance<const_iterator>(begin, end);
    auto       dbegin       = this->begin() + std::distance<const_iterator>(this->begin(), begin);
    auto       dend         = this->begin() + std::distance<const_iterator>(this->begin(), end);
    auto       result_it    = this->begin() + start_offset;
    if constexpr (!std::is_trivially_destructible_v<T>) std::destroy(dbegin, dend);
    std::move(result_it + delta, this->end(), result_it);
    reset_storage(this->data(), this->size() - delta);
    return result_it;
}
template<typename T>
typename mapped<T>::value_type& mapped<T>::front()
{
    return this->data()[0];
}
template<typename T>
typename mapped<T>::value_type& mapped<T>::back()
{
    return this->data()[this->size() - 1];
}
template<typename T>
const typename mapped<T>::value_type& mapped<T>::front() const
{
    return this->data()[0];
}
template<typename T>
const typename mapped<T>::value_type& mapped<T>::back() const
{
    return this->data()[this->size() - 1];
}
template<typename T>
typename mapped<T>::size_type mapped<T>::capacity() const noexcept
{
    return _capacity;
}
template<typename T>
void mapped<T>::shrink_to_fit()
{
    allocate(this->size(), true);
}
template<typename T>
void mapped<T>::clear()
{
    if constexpr (!std::is_trivially_destructible_v<T>) std::destroy(this->begin(), this->end());
    reset_storage(this->data(), 0);
}
template<typename T>
typename mapped<T>::iterator mapped<T>::insert(const_iterator at, T const& value)
{
    auto const delta        = 1;
    auto const start_offset = std::distance<const_iterator>(this->begin(), at);
    auto const move_section = std::distance<const_iterator>(at, this->end());
    auto const new_size     = this->size() + delta;
    while (new_size > _capacity) allocate(std::max(2 * _capacity, 1ll));
    reset_storage(this->data(), this->size() + delta);
    auto result_it = this->begin() + start_offset;
    std::move_backward(result_it, result_it + move_section, result_it + move_section);
    *result_it = std::forward<T const&>(value);
    return this->begin() + start_offset;
}
template<typename T>
typename mapped<T>::iterator mapped<T>::insert(const_iterator at, T&& value)
{
    auto const delta        = 1;
    auto const start_offset = std::distance<const_iterator>(this->begin(), at);
    auto const move_section = std::distance<const_iterator>(at, this->end());
    auto const new_size     = this->size() + delta;
    while (new_size > _capacity) allocate(std::max(2 * _capacity, 1ll));
    reset_storage(this->data(), this->size() + delta);
    auto result_it = this->begin() + start_offset;
    std::move_backward(result_it, result_it + move_section, result_it + move_section);
    *result_it = std::forward<T&&>(value);
    return result_it;
}
template<typename T>
const mygl::buffer& mapped<T>::get_buffer() const
{
    return _buffer;
}
template<typename T>
void mapped<T>::allocate(size_type capacity, bool force)
{
    if (force || _capacity < capacity)
    {
        auto         r = glGetError();
        mygl::buffer new_buffer;
        glCreateBuffers(1, &new_buffer);

        r = glGetError();

        _capacity = capacity;

        glNamedBufferStorage(new_buffer, sizeof(value_type) * capacity, nullptr,
                             GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_CLIENT_STORAGE_BIT
                                 | GL_DYNAMIC_STORAGE_BIT);
        value_type* new_storage = static_cast<value_type*>(glMapNamedBufferRange(new_buffer, 0, sizeof(value_type) * _capacity,
                                                      GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT));
        std::move(this->begin(), this->end(), new_storage);
        reset_storage(new_storage, this->size());

        if (glIsBuffer(_buffer)) glDeleteBuffers(1, &_buffer);
        _buffer = new_buffer;
    }
}
template<typename T>
void mapped<T>::init_range(iterator begin, iterator end, T&& value)
{
    if (end > begin) std::fill(begin, end, value);
}
template<typename T>
void mapped<T>::init_range(iterator begin, iterator end, T const& value)
{
    if (end > begin) std::fill(begin, end, value);
}
template<typename T>
void mapped<T>::reset_storage(value_type* storage, size_type size) noexcept
{
    gsl::span<T>::operator=(gsl::span<T>{storage, static_cast<ptrdiff_t>(size)});
}
template<typename T>
template<typename>
mapped<T>::mapped(size_type size) : mapped()
{
    resize(size);
}
template<typename T>
template<typename... Args>
T& mapped<T>::emplace_back(Args&&... args)
{
    size_type new_cap = _capacity;
    while (this->size() + 1 > new_cap) new_cap *= 2;
    allocate(std::max(new_cap, 1ll));
    reset_storage(this->data(), this->size() + 1);
    return *new (&*std::prev(this->end())) value_type(std::forward<Args&&>(args)...);
}
template<typename T>
template<typename>
void mapped<T>::resize(size_type size)
{
    resize(size, T());
}
template<typename T>
template<class InputIt>
typename mapped<T>::iterator mapped<T>::insert(const_iterator at, InputIt begin, InputIt end)
{
    auto const delta        = std::distance(begin, end);
    auto const start_offset = std::distance<const_iterator>(this->begin(), at);
    auto const move_section = std::distance<const_iterator>(at, this->end());
    auto const new_size     = this->size() + delta;
    while (new_size > _capacity) allocate(std::max(2 * _capacity, 1ll));
    reset_storage(this->data(), this->size() + delta);
    auto result_it = this->begin() + start_offset;
    std::move_backward(result_it, result_it + move_section, result_it + move_section);
    std::copy(begin, end, result_it);
    return result_it;
}

template<typename T>
buffer<T>::buffer(mapped<T> const& source)
{
    glCreateBuffers(1, &_buffer);
    glNamedBufferStorage(_buffer, source.size() * sizeof(T), source.data(), GL_DYNAMIC_STORAGE_BIT);
    _size     = source.size();
    _capacity = source.size();
}

template<typename T>
buffer<T>::buffer(vk::ArrayProxy<T const> const& proxy)
{
    glCreateBuffers(1, &_buffer);
    glNamedBufferStorage(_buffer, proxy.size() * sizeof(T), proxy.data(), GL_DYNAMIC_STORAGE_BIT);
    _size     = proxy.size();
    _capacity = proxy.size();
}

template<typename T>
buffer<T>::buffer(std::initializer_list<T> const& source)
{
    glCreateBuffers(1, &_buffer);
    glNamedBufferStorage(_buffer, std::size(source) * sizeof(T), std::data(source), GL_DYNAMIC_STORAGE_BIT);
    _size     = std::size(source);
    _capacity = std::size(source);
}

template<typename T>
buffer<T>::~buffer()
{
    if (glIsBuffer(_buffer)) glDeleteBuffers(1, &_buffer);
}

template<typename T>
buffer<T>::buffer(buffer const& other)
{
    glCreateBuffers(1, &_buffer);
    glNamedBufferStorage(_buffer, other.size() * sizeof(T), nullptr, GL_DYNAMIC_STORAGE_BIT);
    glCopyNamedBufferSubData(other._buffer, _buffer, 0, 0, other.size() * sizeof(T));
    _size     = other.size();
    _capacity = other.size();
}

template<typename T>
buffer<T>::buffer(buffer&& other) noexcept
{
    _buffer   = std::move(other._buffer);
    _capacity = other._capacity;
    _size     = other._size;

    other._buffer   = mygl::buffer::zero();
    other._capacity = 0;
    other._size     = 0;
}

template<typename T>
buffer<T>& buffer<T>::operator=(buffer const& other)
{
    if (glIsBuffer(_buffer)) glDeleteBuffers(1, &_buffer);

    glCreateBuffers(1, &_buffer);
    glNamedBufferStorage(_buffer, other.size() * sizeof(T), nullptr, GL_DYNAMIC_STORAGE_BIT);
    glCopyNamedBufferSubData(other._buffer, _buffer, 0, 0, other.size() * sizeof(T));
    _size     = other.size();
    _capacity = other.size();
    return *this;
}

template<typename T>
buffer<T>& buffer<T>::operator=(buffer&& other) noexcept
{
    _buffer   = std::move(other._buffer);
    _capacity = other._capacity;
    _size     = other._size;

    other._buffer   = mygl::buffer::zero();
    other._capacity = 0;
    other._size     = 0;
    return *this;
}

template<typename T>
mapped<T> buffer<T>::download() const
{
    mapped<T> result(_size);
    glCopyNamedBufferSubData(_buffer, result.get_buffer(), 0, 0, _size * sizeof(T));
    return result;
}

template<typename T>
typename buffer<T>::size_type buffer<T>::size() const
{
    return _size;
}

template<typename T>
typename buffer<T>::size_type buffer<T>::capacity() const
{
    return _capacity;
}

template<typename T>
const mygl::buffer& buffer<T>::get_buffer() const
{
    return _buffer;
}

template<typename T>
void buffer<T>::update(mapped<T> const& source)
{
    if (source.size() > capacity() || !glIsBuffer(_buffer))
    {
        if (glIsBuffer(_buffer)) glDeleteBuffers(1, &_buffer);

        glCreateBuffers(1, &_buffer);
        glNamedBufferStorage(_buffer, source.capacity() * sizeof(T), nullptr, GL_DYNAMIC_STORAGE_BIT);
        _capacity = source.capacity();
    }

    glCopyNamedBufferSubData(source.get_buffer(), _buffer, 0, 0, source.size() * sizeof(T));
    _size = source.size();
}
}    // namespace v1
}    // namespace gl
}    // namespace gfx
