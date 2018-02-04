
#include "device.hpp"
#include "queue_filter.hpp"
#include "memory.hpp"
#include "detail.hpp"

namespace vkn
{
    device::device(vk::PhysicalDevice gpu, vk::SurfaceKHR surface, vk::QueueFlags enabled_queues,
        uint32_t memory_block_size)
        : _gpu(gpu), _surface(surface)
    {
        std::vector<QueueFilter> filters;
        if ((enabled_queues & vk::QueueFlagBits::eGraphics) == vk::QueueFlagBits::eGraphics)
            filters.push_back(QueueFilter(1.f, [&](int32_t fam, const auto& props) -> bool {
            return _gpu.getSurfaceSupportKHR(fam, _surface) && (props.queueFlags & vk::QueueFlagBits::eGraphics) == vk::QueueFlagBits::eGraphics;
        }));
        if ((enabled_queues & vk::QueueFlagBits::eCompute) == vk::QueueFlagBits::eCompute)
            filters.push_back(QueueFilter(1.f, vk::QueueFlagBits::eCompute));
        if ((enabled_queues & vk::QueueFlagBits::eTransfer) == vk::QueueFlagBits::eTransfer)
            filters.push_back(QueueFilter(0.8f, vk::QueueFlagBits::eTransfer));
        QueueCreator queue_creator(_gpu, filters);

        vk::DeviceCreateInfo device_info;
        device_info.setEnabledExtensionCount(static_cast<uint32_t>(detail::device_extensions.size()))
            .setPpEnabledExtensionNames(detail::device_extensions.data());

#if defined(NDEBUG)
#else
        device_info.setEnabledLayerCount(static_cast<uint32_t>(detail::layers.size()))
            .setPpEnabledLayerNames(detail::layers.data());
#endif

        device_info.setQueueCreateInfoCount(static_cast<uint32_t>(queue_creator.createInfos().size()))
            .setPQueueCreateInfos(queue_creator.createInfos().data());
        const auto features = _gpu.getFeatures();
        device_info.setPEnabledFeatures(&features);

        Device::operator=(_gpu.createDevice(device_info));

        if (!*this) throw std::runtime_error("Could not create device");

        //Query Queues and also create command buffers
        const auto make_queue = [this](uint32_t family) -> queue_info {
            return { getQueue(family, 0), family };
        };

        if ((enabled_queues & vk::QueueFlagBits::eGraphics) == vk::QueueFlagBits::eGraphics)
        {
            _command_pools[vk::QueueFlagBits::eGraphics] = createCommandPool({ vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
                (_queues[vk::QueueFlagBits::eGraphics] = make_queue(queue_creator.families()[0])).family });
        }
        if ((enabled_queues & vk::QueueFlagBits::eCompute) == vk::QueueFlagBits::eCompute)
        {
            _command_pools[vk::QueueFlagBits::eCompute] = createCommandPool({ vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
                (_queues[vk::QueueFlagBits::eCompute] = make_queue(queue_creator.families()[1])).family });
        }
        if ((enabled_queues & vk::QueueFlagBits::eTransfer) == vk::QueueFlagBits::eTransfer)
        {
            _command_pools[vk::QueueFlagBits::eTransfer] = createCommandPool({ {},
                (_queues[vk::QueueFlagBits::eTransfer] = make_queue(queue_creator.families()[2])).family });
        }

        _unique_families = queue_creator.uniqueFamilies();
        _memory_pool = jpu::make_ref<memory_pool>(this, memory_block_size);
    }

    device::~device()
    {
        for (auto&& command_pool : _command_pools)
            destroyCommandPool(command_pool.second);
        assert(_memory_pool->references() == 1 && "Found unused Reference to the device local memory pool. This is invalid usage.");
        _memory_pool.reset();
        destroy();
    }

    const queue_info& device::queue(vk::QueueFlagBits type) const
    {
        return _queues.at(type);
    }

    const vk::CommandPool& device::command_pool(vk::QueueFlagBits type) const
    {
        return _command_pools.at(type);
    }

    const std::vector<uint32_t>& device::families() const
    {
        return _unique_families;
    }

    void device::unique_command(vk::QueueFlagBits queue_type, std::function<void(vk::CommandBuffer)> runnable,
        vk::Fence fence) const
    {
        auto cmdbuf = allocateCommandBuffers(vk::CommandBufferAllocateInfo(command_pool(queue_type), vk::CommandBufferLevel::ePrimary, 1));
        cmdbuf[0].begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
        {
            runnable(cmdbuf[0]);
        }
        cmdbuf[0].end();

        queue(queue_type).queue.submit({ vk::SubmitInfo(0, nullptr, nullptr, 1, cmdbuf.data()) }, fence);
    }

    vk::PhysicalDevice device::gpu() const
    {
        return _gpu;
    }

    memory_pool* device::memory() const
    {
        return _memory_pool.get();
    }
}
