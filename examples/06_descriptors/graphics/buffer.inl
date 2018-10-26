#include "buffer.hpp"
#pragma once

namespace gfx {
namespace v1 {

template<typename T>
mapped<T>::mapped(device& d) : _device(&d)
{
    std::unordered_set<u32> families;
    families.emplace(_device->compute_family());
    families.emplace(_device->graphics_family());
    families.emplace(_device->transfer_family());
    _families = std::vector<u32>(families.begin(), families.end());
}
template<typename T>
mapped<T>::mapped(device& d, size_type size, T&& value) : mapped(d)
{
    resize(size, std::forward<T&&>(value));
}
template<typename T>
mapped<T>::mapped(device& d, std::initializer_list<T> ilist) : mapped(d)
{
    reserve(std::size(ilist));
    reset_storage(this->data(), std::size(ilist));
    std::move(std::begin(ilist), std::end(ilist), this->begin());
}
template<typename T>
mapped<T>::mapped(const mapped& other)
{
    allocate(other._capacity);
    reset_storage(this->data(), other.size());
    std::copy(other.begin(), other.end(), this->begin());
}
template<typename T>
mapped<T>::mapped(mapped&& other) noexcept
{
    reset_storage(other.data(), other.size());
    _families   = std::move(other._families);
    _buffer     = std::move(other._buffer);
    _allocation = std::move(other._allocation);
    _capacity   = other._capacity;
    _device     = other._device;

    other.reset_storage(nullptr, 0);
    other._buffer     = nullptr;
    other._allocation = nullptr;
    other._capacity   = 0;
}
template<typename T>
mapped<T>& mapped<T>::operator=(const mapped<T>& other)
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
    _families   = std::move(other._families);
    _buffer     = std::move(other._buffer);
    _allocation = std::move(other._allocation);
    _capacity   = other._capacity;
    _device     = other._device;

    other.reset_storage(nullptr, 0);
    other._buffer     = nullptr;
    other._allocation = nullptr;
    other._capacity   = 0;
    return *this;
}
template<typename T>
mapped<T>::~mapped()
{
    if constexpr (!std::is_trivially_destructible_v<T>) std::destroy(this->begin(), this->end());
    if (_buffer) vmaDestroyBuffer(_device->get_allocator(), _buffer, _allocation);
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
    const auto old_size = this->size();

    reset_storage(this->data(), size);
    if (size > old_size) init_range(this->begin() + old_size, this->end(), std::forward<T&&>(value));
}
template<typename T>
void mapped<T>::resize(size_type size, const T& value)
{
    if constexpr (!std::is_trivially_destructible_v<T>)
        if (size < this->size()) std::destroy(this->begin() + size, this->end());
    while (size > _capacity) allocate(std::max(2 * _capacity, 1ll));
    const auto old_size = this->size();
    reset_storage(this->data(), size);
    if (size > old_size) init_range(this->begin() + old_size, this->end(), std::forward<const T&>(value));
}
template<typename T>
void mapped<T>::push_back(T&& value)
{
    resize(this->size() + 1, std::forward<T&&>(value));
}
template<typename T>
void mapped<T>::push_back(const T& value)
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
    const auto start_offset = std::distance<const_iterator>(this->begin(), at);
    auto       result_it    = this->begin() + start_offset;
    if constexpr (!std::is_trivially_destructible_v<T>) std::destroy_at(std::addressof(*result_it));
    std::move(std::next(result_it), this->end(), result_it);
    reset_storage(this->data(), this->size() - 1);
    return result_it;
}
template<typename T>
typename mapped<T>::iterator mapped<T>::erase(const_iterator begin, const_iterator end)
{
    const auto start_offset = std::distance<const_iterator>(this->begin(), begin);
    const auto delta        = std::distance(begin, end);
    auto       dbegin       = this->begin() + std::distance(this->begin(), begin);
    auto       dend         = this->begin() + std::distance(this->begin(), end);
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
typename mapped<T>::size_type mapped<T>::capacity() const
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
typename mapped<T>::iterator mapped<T>::insert(const_iterator at, const T& value)
{
    const auto delta        = 1;
    const auto start_offset = std::distance<const_iterator>(this->begin(), at);
    const auto move_section = std::distance<const_iterator>(at, this->end());
    const auto new_size     = this->size() + delta;
    while (new_size > _capacity) allocate(std::max(2 * _capacity, 1ll));
    reset_storage(this->data(), this->size() + delta);
    auto result_it = this->begin() + start_offset;
    std::move_backward(result_it, result_it + move_section, result_it + move_section);
    *result_it = std::forward<const T&>(value);
    return this->begin() + start_offset;
}
template<typename T>
typename mapped<T>::iterator mapped<T>::insert(const_iterator at, T&& value)
{
    const auto delta        = 1;
    const auto start_offset = std::distance<const_iterator>(this->begin(), at);
    const auto move_section = std::distance<const_iterator>(at, this->end());
    const auto new_size     = this->size() + delta;
    while (new_size > _capacity) allocate(std::max(2 * _capacity, 1ll));
    reset_storage(this->data(), this->size() + delta);
    auto result_it = this->begin() + start_offset;
    std::move_backward(result_it, result_it + move_section, result_it + move_section);
    *result_it = std::forward<T&&>(value);
    return result_it;
}
template<typename T>
const vk::Buffer& mapped<T>::get_buffer() const
{
    return _buffer;
}
template<typename T>
void mapped<T>::allocate(size_type capacity, bool force)
{
    if (force || _capacity < capacity)
    {
        _capacity = capacity;
        VmaAllocationInfo       ai{};
        VmaAllocationCreateInfo aci{};
        aci.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
        aci.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
        using bu  = vk::BufferUsageFlagBits;
        vk::BufferCreateInfo bci(
            {}, sizeof(value_type) * capacity,
            bu::eIndexBuffer | bu::eIndirectBuffer | bu::eStorageBuffer
                | bu::eStorageTexelBuffer
                /*|  bu::eConditionalRenderingEXT | bu::eRaytracingNVX | bu::eRaytracingNVX*/
                | bu::eTransferDst | bu::eTransferSrc | bu::eUniformBuffer | bu::eUniformTexelBuffer | bu::eVertexBuffer,
            _families.size() == 1 ? vk::SharingMode::eExclusive : vk::SharingMode::eConcurrent, u32(_families.size()), _families.data());
        VkBuffer      nb;
        VmaAllocation na;
        vmaCreateBuffer(_device->get_allocator(), reinterpret_cast<VkBufferCreateInfo*>(&bci), &aci, &nb, &na, &ai);
        value_type* new_storage = static_cast<value_type*>(ai.pMappedData);
        std::move(this->begin(), this->end(), new_storage);
        reset_storage(new_storage, this->size());

        vmaDestroyBuffer(_device->get_allocator(), _buffer, _allocation);
        _buffer     = nb;
        _allocation = na;
    }
}
template<typename T>
void mapped<T>::init_range(iterator begin, iterator end, T&& value)
{
    if (end > begin) std::fill(begin, end, value);
}
template<typename T>
void mapped<T>::reset_storage(value_type* storage, size_type size)
{
    gsl::span<T>::operator=(gsl::span<T>{storage, static_cast<ptrdiff_t>(size)});
}
template<typename T>
template<typename>
mapped<T>::mapped(device& d, size_type size) : mapped(d)
{
    resize(size);
}
template<typename T>
template<typename... Args>
T& mapped<T>::emplace_back(Args&&... args)
{
    while (this->size() + 1 > _capacity) allocate(std::max(2 * _capacity, 1ll));
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
    const auto delta        = std::distance(begin, end);
    const auto start_offset = std::distance<const_iterator>(this->begin(), at);
    const auto move_section = std::distance<const_iterator>(at, this->end());
    const auto new_size     = this->size() + delta;
    while (new_size > _capacity) allocate(std::max(2 * _capacity, 1ll));
    reset_storage(this->data(), this->size() + delta);
    auto result_it = this->begin() + start_offset;
    std::move_backward(result_it, result_it + move_section, result_it + move_section);
    std::copy(begin, end, result_it);
    return result_it;
}


template<typename T>
buffer<T>::buffer(device& d) : _device(&d)
{
    std::unordered_set<u32> families;
    families.emplace(_device->compute_family());
    families.emplace(_device->graphics_family());
    families.emplace(_device->transfer_family());
    _families = std::vector<u32>(families.begin(), families.end());
}

template<typename T>
buffer<T>::buffer(device& d, const mapped<T>& source) : buffer(d)
{
    commands transfer_cmd = d.allocate_transfer_command();
    transfer_cmd.cmd().begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    allocate(source.size(), false, transfer_cmd.cmd());
    _size = source.size();
    transfer_cmd.copy(source, *this);
    transfer_cmd.cmd().end();
    d.transfer_queue().submit({transfer_cmd}, {}, {});
    d.transfer_queue().wait();
}

template<typename T>
buffer<T>::buffer(device& d, const mapped<T>& source, commands& transfer_cmd) : buffer(d)
{
    allocate(source.size(), false, transfer_cmd.cmd());
    _size = source.size();
    transfer_cmd.cmd().copyBuffer(source.get_buffer(), _buffer, vk::BufferCopy(0, 0, _size));
}

template<typename T>
buffer<T>::buffer(device& d, const vk::ArrayProxy<const T>& proxy) : buffer(d)
{
    const auto bytes        = proxy.size() * sizeof(T);
    commands   transfer_cmd = d.allocate_transfer_command();
    transfer_cmd.cmd().begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    allocate(proxy.size(), false, transfer_cmd.cmd());
    _size = proxy.size();
    mapped<T> stage(d);
    if (bytes <= 65536) { transfer_cmd.cmd().updateBuffer(_buffer, 0, bytes, std::data(proxy)); }
    else
    {
        stage.insert(stage.end(), proxy.begin(), proxy.end());
        transfer_cmd.copy(stage, *this);
    }
    transfer_cmd.cmd().end();
    d.transfer_queue().submit({transfer_cmd}, {}, {});
    d.transfer_queue().wait();
}

template<typename T>
buffer<T>::buffer(device& d, const vk::ArrayProxy<const T>& proxy, commands& transfer_cmd) : buffer(d)
{
    const auto bytes = proxy.size() * sizeof(T);
    allocate(proxy.size(), false, transfer_cmd.cmd());
    _size = proxy.size();
    mapped<T> stage(d);
    if (bytes <= 65536) { transfer_cmd.cmd().updateBuffer(_buffer, 0, bytes, std::data(proxy)); }
    else
    {
        stage.insert(stage.end(), proxy.begin(), proxy.end());
        transfer_cmd.copy(stage, *this);
    }
}

template<typename T>
buffer<T>::buffer(device& d, const std::initializer_list<T>& source) : buffer(d)
{
    const auto bytes        = std::size(source) * sizeof(T);
    commands   transfer_cmd = d.allocate_transfer_command();
    transfer_cmd.cmd().begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    allocate(source.size(), false, transfer_cmd.cmd());
    _size = source.size();
    if (bytes <= 65536) { transfer_cmd.cmd().updateBuffer(_buffer, 0, bytes, std::data(source)); }
    else
    {
        mapped<T> stage(d, source);
        transfer_cmd.copy(stage, *this);
    }
    transfer_cmd.cmd().end();
    d.transfer_queue().submit({transfer_cmd}, {}, {});
    d.transfer_queue().wait();
}

template<typename T>
buffer<T>::buffer(device& d, const std::initializer_list<T>& source, commands& transfer_cmd) : buffer(d)
{
    const auto bytes = std::size(source) * sizeof(T);
    allocate(source.size(), false, transfer_cmd.cmd());
    _size = source.size();
    if (bytes <= 65536) { transfer_cmd.cmd().updateBuffer(_buffer, 0, bytes, std::data(source)); }
    else
    {
        mapped<T> stage(d, source);
        transfer_cmd.copy(stage, *this);
    }
}

template<typename T>
buffer<T>::~buffer()
{
    if (_buffer) vmaDestroyBuffer(_device->get_allocator(), _buffer, _allocation);
}

template<typename T>
buffer<T>::buffer(const buffer& other) : buffer(*other._device)
{
    commands transfer_cmd = _device->allocate_transfer_command();
    transfer_cmd.cmd().begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    allocate(other.size(), false, transfer_cmd.cmd());
    _size = other.size();
    transfer_cmd.copy(other, *this);
    transfer_cmd.cmd().end();
    _device->transfer_queue().submit({transfer_cmd}, {}, {});
    _device->transfer_queue().wait();
}

template<typename T>
buffer<T>::buffer(buffer&& other) noexcept
{
    _buffer     = std::move(other._buffer);
    _allocation = std::move(other._allocation);
    _capacity   = other._capacity;
    _size       = other._size;
    _device     = other._device;
    _families   = std::move(other._families);

    other._buffer     = nullptr;
    other._allocation = nullptr;
    other._capacity   = 0;
    other._size       = 0;
    other._device     = nullptr;
}

template<typename T>
buffer<T>& buffer<T>::operator=(const buffer& other)
{
    _device               = other._device;
    commands transfer_cmd = _device->allocate_transfer_command();
    transfer_cmd.cmd().begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    allocate(other.size(), false, transfer_cmd.cmd());
    _size = other.size();
    transfer_cmd.copy(other, *this);
    transfer_cmd.cmd().end();
    _device->transfer_queue().submit({transfer_cmd}, {}, {});
    _device->transfer_queue().wait();
    return *this;
}

template<typename T>
buffer<T>& buffer<T>::operator=(buffer&& other) noexcept
{
    _buffer     = std::move(other._buffer);
    _allocation = std::move(other._allocation);
    _capacity   = other._capacity;
    _size       = other._size;
    _device     = other._device;
    _families   = std::move(other._families);

    other._buffer     = nullptr;
    other._allocation = nullptr;
    other._capacity   = 0;
    other._size       = 0;
    other._device     = nullptr;
    return *this;
}

template<typename T>
mapped<T> buffer<T>::download() const
{
    mapped<T> result(*_device, _size);
    commands  transfer_cmd = _device->allocate_transfer_command();
    transfer_cmd.cmd().begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    transfer_cmd.copy(*this, result);
    transfer_cmd.cmd().end();
    _device->transfer_queue().submit({transfer_cmd}, {}, {});
    _device->transfer_queue().wait();
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
const vk::Buffer& buffer<T>::get_buffer() const
{
    return _buffer;
}

template<typename T>
void buffer<T>::allocate(size_type capacity, bool force, vk::CommandBuffer copy_cmd)
{
    if (force || _capacity < capacity)
    {
        _capacity = capacity;
        VmaAllocationInfo       ai{};
        VmaAllocationCreateInfo aci{};
        aci.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        using bu  = vk::BufferUsageFlagBits;
        vk::BufferCreateInfo bci(
            {}, sizeof(value_type) * capacity,
            bu::eIndexBuffer | bu::eIndirectBuffer | bu::eStorageBuffer
                | bu::eStorageTexelBuffer
                /*|  bu::eConditionalRenderingEXT | bu::eRaytracingNVX | bu::eRaytracingNVX*/
                | bu::eTransferDst | bu::eTransferSrc | bu::eUniformBuffer | bu::eUniformTexelBuffer | bu::eVertexBuffer,
            _families.size() == 1 ? vk::SharingMode::eExclusive : vk::SharingMode::eConcurrent, u32(_families.size()), _families.data());
        VkBuffer      nb;
        VmaAllocation na;
        vmaCreateBuffer(_device->get_allocator(), reinterpret_cast<VkBufferCreateInfo*>(&bci), &aci, &nb, &na, &ai);

        if (_buffer) copy_cmd.copyBuffer(_buffer, nb, vk::BufferCopy(0, 0, _size));

        vmaDestroyBuffer(_device->get_allocator(), _buffer, _allocation);
        _buffer     = nb;
        _allocation = na;
    }
}
}    // namespace v1
}    // namespace gfx