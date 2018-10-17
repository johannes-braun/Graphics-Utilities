#pragma once

#include "types.hpp"
#include <memory>
#include <unordered_set>
#include <vulkan/vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

#include "device.hpp"

namespace gfx {
inline namespace v1 {
template<typename T>
class mapped : public gsl::span<T>
{
public:
    using element_type           = T;
    using value_type             = std::remove_cv_t<T>;
    using index_type             = std::ptrdiff_t;
    using pointer                = element_type*;
    using reference              = element_type&;
    using iterator               = gsl::details::span_iterator<span<T, gsl::dynamic_extent>, false>;
    using const_iterator         = gsl::details::span_iterator<span<T, gsl::dynamic_extent>, true>;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using size_type              = index_type;
    using difference_type        = ptrdiff_t;

    mapped(device& d) : _device(&d)
    {
        std::unordered_set<u32> families;
        families.emplace(_device->compute_family());
        families.emplace(_device->graphics_family());
        families.emplace(_device->transfer_family());
        _families = std::vector<u32>(families.begin(), families.end());
    }

    template<typename = std::enable_if_t<std::is_default_constructible_v<T>>>
    mapped(device& d, size_type size) : mapped(d)
    {
        resize(size);
    }
    mapped(device& d, size_type size, T&& value) : mapped(d) { resize(size, std::forward<T&&>(value)); }

    mapped(const mapped& other)
    {
        allocate(other._capacity);
        reset_storage(this->data(), other.size());
        std::copy(other.begin(), other.end(), this->begin());
    }
    mapped& operator=(const mapped& other)
    {
        std::destroy(this->begin(), this->end());
        allocate(other._capacity);
        reset_storage(this->data(), other.size());
        std::copy(other.begin(), other.end(), this->begin());
        return *this;
    }
    mapped(mapped&& other)
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
    mapped& operator=(mapped&& other) 
	{
		std::destroy(this->begin(), this->end());
		reset_storage(other.data(), other.size());
		_families = std::move(other._families);
		_buffer = std::move(other._buffer);
		_allocation = std::move(other._allocation);
		_capacity = other._capacity;
		_device = other._device;

		other.reset_storage(nullptr, 0);
		other._buffer = nullptr;
		other._allocation = nullptr;
		other._capacity = 0;
	}
	~mapped()
	{
		std::destroy(this->begin(), this->end());
		if (_buffer)
			vmaDestroyBuffer(_device->alloc(), _buffer, _allocation);
	}

    void reserve(size_type capacity) { allocate(capacity); }

    void resize(size_type size, T&& value)
    {
        while (size > _capacity) allocate(std::max(2 * _capacity, 1ll));
        const auto old_size = this->size();
        reset_storage(this->data(), size);
        if (size > old_size) init_range(this->begin() + old_size, this->end(), std::forward<T&&>(value));
    }

    void resize(size_type size, const T& value)
    {
        while (size > _capacity) allocate(std::max(2 * _capacity, 1ll));
        const auto old_size = this->size();
        reset_storage(this->data(), size);
        if (size > old_size) init_range(this->begin() + old_size, this->end(), std::forward<const T&>(value));
    }

    void push_back(T&& value) { resize(this->size() + 1, std::forward<T&&>(value)); }
    void push_back(const T& value) { resize(this->size() + 1, std::forward<const T&>(value)); }
    void pop_back()
    {
        std::destroy_at(std::addressof(back()));
        reset_storage(this->data(), this->size() - 1);
    }

    iterator erase(const_iterator at)
    {
        const auto start_offset = std::distance<const_iterator>(this->begin(), at);
        auto       result_it    = this->begin() + start_offset;
        std::destroy_at(std::addressof(*result_it));
        std::move(std::next(result_it), this->end(), result_it);
        reset_storage(this->data(), this->size() - 1);
        return result_it;
    }
    iterator erase(const_iterator begin, const_iterator end)
    {
        const auto start_offset = std::distance<const_iterator>(this->begin(), at);
        const auto delta        = std::distance(begin, end);
        auto       dbegin       = this->begin() + std::distance(this->begin(), begin);
        auto       dend         = this->begin() + std::distance(this->begin(), end);
        auto result_it          = this->begin() + start_offset std::destroy(dbegin, dend);
        std::move(result_it + delta, this->end(), result_it);
        reset_storage(this->data(), this->size() - delta);
        return result_it;
    }

    template<typename... Args>
    T& emplace_back(Args&&... args)
    {
        while (this->size() + 1 > _capacity) allocate(std::max(2 * _capacity, 1ll));
        reset_storage(this->data(), this->size() + 1);
        return *new (&*std::prev(this->end())) value_type(std::forward<Args&&>(args)...);
    }

    template<typename = std::enable_if_t<std::is_default_constructible_v<T>>>
    void resize(size_type size)
    {
        resize(size, T());
    }

    value_type&       front() { return this->data()[0]; }
    value_type&       back() { return this->data()[this->size() - 1]; }
    const value_type& front() const { return this->data()[0]; }
    const value_type& back() const { return this->data()[this->size() - 1]; }
    size_type         capacity() const { return _capacity; }
    void              shrink_to_fit() { allocate(this->size(), true); }
    void              clear() { std::destroy(this->begin(), this->end()); }

    iterator insert(const_iterator at, const T& value)
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
    iterator insert(const_iterator at, T&& value)
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
    template<class InputIt>
    iterator insert(const_iterator at, InputIt begin, InputIt end)
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
    iterator insert(const_iterator at, std::initializer_list<T> ilist) { return insert(at, std::begin(ilist), std::end(ilist)); }

private:
    void allocate(size_type capacity, bool force = false)
    {
        if (force || _capacity < capacity)
        {
            _capacity = capacity;
            VmaAllocationInfo       ai{};
            VmaAllocationCreateInfo aci{};
            aci.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
            aci.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
            using bu  = vk::BufferUsageFlagBits;
            vk::BufferCreateInfo bci({}, capacity,
                                     bu::eConditionalRenderingEXT | bu::eIndexBuffer | bu::eIndirectBuffer | bu::eRaytracingNVX
                                         | bu::eRaytracingNVX | bu::eStorageBuffer | bu::eStorageTexelBuffer | bu::eTransferDst
                                         | bu::eTransferSrc | bu::eUniformBuffer | bu::eUniformTexelBuffer | bu::eVertexBuffer,
                                     _families.size() == 1 ? vk::SharingMode::eExclusive : vk::SharingMode::eConcurrent,
                                     u32(_families.size()), _families.data());
            VkBuffer             nb;
            VmaAllocation        na;
            vmaCreateBuffer(_device->alloc(), reinterpret_cast<VkBufferCreateInfo*>(&bci), &aci, &nb, &na, &ai);
            value_type* new_storage = static_cast<value_type*>(ai.pMappedData);
            std::move(this->begin(), this->end(), new_storage);
            reset_storage(new_storage, this->size());

            vmaDestroyBuffer(_device->alloc(), _buffer, _allocation);
            _buffer     = nb;
            _allocation = na;
        }
    }

    void init_range(iterator begin, iterator end, T&& value)
    {
        if (end > begin) std::fill(begin, end, value);
    }

    void reset_storage(value_type* storage, size_type size)
    {
        gsl::span<T>::operator=(gsl::span<T>{storage, static_cast<ptrdiff_t>(size)});
    }

    std::vector<u32> _families;
    vk::Buffer       _buffer     = nullptr;
    VmaAllocation    _allocation = nullptr;
    size_type        _capacity   = 0;
    device*          _device     = nullptr;
};
}    // namespace v1
}    // namespace gfx