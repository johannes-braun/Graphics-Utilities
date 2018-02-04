#pragma once

#include <vulkan/vulkan.hpp>
#include <jpu/memory>
#include "device.hpp"

namespace vkn
{
    class descriptor_set_layout : public jpu::ref_count
    {
    public:
        descriptor_set_layout(device* device, std::vector<vk::DescriptorSetLayoutBinding> bindings,
            vk::DescriptorSetLayoutCreateFlags flags = vk::DescriptorSetLayoutCreateFlags());

        ~descriptor_set_layout();

        operator vk::DescriptorSetLayout() const;

    private:
        device * _device;
        vk::DescriptorSetLayout _layout;
        vk::DescriptorSetLayoutCreateFlags _flags;
        std::vector<vk::DescriptorSetLayoutBinding> _bindings;
    };

    class descriptor_pool : public jpu::ref_count
    {
    public:
        descriptor_pool(device* device, uint32_t max_count, std::vector<vk::DescriptorPoolSize> sizes);
        ~descriptor_pool();

        vk::DescriptorSet allocate(descriptor_set_layout* layout) const;

        operator vk::DescriptorPool() const;

    private:
        vk::DescriptorPool _pool;
        device * _device;
        uint32_t _max_count;
        std::vector<vk::DescriptorPoolSize> _sizes;
    };
}
