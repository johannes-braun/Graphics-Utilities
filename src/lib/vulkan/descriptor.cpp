#include "descriptor.hpp"

namespace vkn
{
    descriptor_set_layout::descriptor_set_layout(device* device, std::vector<vk::DescriptorSetLayoutBinding> bindings,
        vk::DescriptorSetLayoutCreateFlags flags) : _device(device),
        _flags(flags),
        _bindings(bindings)
    {
        _device->inc_ref();
        _layout = _device->createDescriptorSetLayout(
            vk::DescriptorSetLayoutCreateInfo(_flags, static_cast<uint32_t>(std::size(_bindings)),
                std::data(_bindings)));
    }

    descriptor_set_layout::~descriptor_set_layout()
    {
        _device->destroyDescriptorSetLayout(_layout);
        _device->dec_ref();
    }

    descriptor_set_layout::operator vk::DescriptorSetLayout() const
    {
        return _layout;
    }

    descriptor_pool::descriptor_pool(device* device, uint32_t max_count, std::vector<vk::DescriptorPoolSize> sizes)
        : _device(device), _max_count(max_count), _sizes(sizes)
    {
        _device->inc_ref();
        _pool = _device->createDescriptorPool(vk::DescriptorPoolCreateInfo({}, _max_count, static_cast<uint32_t>(std::size(_sizes)), std::data(_sizes)));
    }

    descriptor_pool::~descriptor_pool()
    {
        _device->destroyDescriptorPool(_pool);
        _device->dec_ref();
    }

    vk::DescriptorSet descriptor_pool::allocate(descriptor_set_layout* layout) const
    {
        vk::DescriptorSetLayout l = *layout;
        return _device->allocateDescriptorSets(vk::DescriptorSetAllocateInfo(_pool, 1, &l))[0];
    }

    descriptor_pool::operator vk::DescriptorPool() const
    {
        return _pool;
    }
}
