#pragma once

#include <array>
#include <vulkan/vulkan.hpp>

namespace gfx::eng::core
{
struct family : public vk::Queue, public vk::CommandPool
{
    enum type
    {
        graphics = 0,
        compute,
        transfer,
        present,

        __count
    };

    family() = default;

    family(const vk::UniqueDevice& device, uint32_t family, uint32_t index)
            : vk::Queue(device->getQueue(family, index))
            , index(family)
            , vk::CommandPool(
                      device->createCommandPool(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, family)))
            , _device(device.get())
    {
    }

    ~family()
    {
        if(_device)
            _device.destroyCommandPool(*this);
    }

    operator uint32_t() const { return index; }

    const uint32_t index;

private:
    vk::Device _device;
};

using family_array = std::array<family, family::__count>;

} // namespace gfx::eng::core