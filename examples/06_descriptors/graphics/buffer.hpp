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
    using iterator               = gsl::details::span_iterator<span<T, gsl::span<T>::Extent>, false>;
    using const_iterator         = gsl::details::span_iterator<span<T, gsl::span<T>::Extent>, true>;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using size_type              = size_t;
    using difference_type        = ptrdiff_t;

    mapped()
    {
        std::unordered_set<u32> families;
        families.emplace(_device->compute_family());
        families.emplace(_device->graphics_family());
        families.emplace(_device->transfer_family());
        _families = std::vector<u32>(families.begin(), families.end());
    }


private:
	void reserve_larger(size_type size)
	{

	}

    void resize_larger(size_type size, T&& value)
    {
        const auto              old_size = this->size();
        VmaAllocationInfo       ai{};
        VmaAllocationCreateInfo aci{};
        aci.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
        aci.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
        using bu  = vk::BufferUsageFlagBits;
        vk::BufferCreateInfo bci({}, size,
                                 bu::eConditionalRenderingEXT | bu::eIndexBuffer | bu::eIndirectBuffer | bu::eRaytracingNVX
                                     | bu::eRaytracingNVX | bu::eStorageBuffer | bu::eStorageTexelBuffer | bu::eTransferDst
                                     | bu::eTransferSrc | bu::eUniformBuffer | bu::eUniformTexelBuffer | bu::eVertexBuffer,
                                 _families.size() == 1 ? vk::SharingMode::eExclusive : vk::SharingMode::eConcurrent, u32(_families.size()),
                                 _families.data());
        VkBuffer             nb;
        VmaAllocation        na;
        vmaCreateBuffer(_device->_instance->alloc(), reinterpret_cast<VkBufferCreateInfo>(&bci), &aci, &nb, &na, &ai);
        value_type* new_storage = static_cast<value_type*>(ai.pMappedData);
        std::move(this->begin(), this->end(), new_storage);
        reset_storage(new_storage, size);

        init_range(this->begin() + old_size, this - end(), std::forward<T&&>(value));
    }

    void init_range(iterator begin, iterator end, T&& value)
    {
        if (end > begin) std::fill(begin, end, value);
    }

    void reset_storage(value_type* storage, size_type size) { gsl::span<T>::operator=(gsl::span<T>{storage, size}); }

    std::vector<u32> _families;
    vk::Buffer       _buffer;
    VmaAllocation    _allocation;
    device*          _device;
};
}    // namespace v1
}    // namespace gfx